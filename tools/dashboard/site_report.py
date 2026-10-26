#!/usr/bin/env python3
"""
The shared portfolio site's report (mitsevox.github.io/games/tw2004/, report-v1): objdiff's
numbers in the site's public input format.

    python tools/dashboard/site_report.py <report.json> <history.json> <out> <sha> <tree>
                                          <run_id> <attempt> <workflow_blob>

Writes <out>/public-input/{summary,map,timeline}.json and <out>/metadata.json, which the site's
tools/package_report.py turns into the uploaded package. Code is measured by function (objdiff's
code totals leave out the padding between functions), data by section. Functions count as named
unless they still carry an fn_XXXXXXXX address name; EA game files count as exact once
docs/audit.tsv marks them done (the audit checks each file's boundaries). The timeline is the
progress history (history.json on pages-history): rows CI wrote with full report.json numbers are inherited as they
are, older rows (backfill_history.py rebuilds of old commits, before totals were recorded) get the
report's current totals and are marked reconstructed. Only report.json numbers are written.
"""
import json, os, re, subprocess, sys, time

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
PROJECT = 'tw2004'
TARGET = {'id': 'GW4E69', 'console': 'GameCube', 'sha1': 'bbbc55485e51973931dee169e7bf87bc7379223f'}
METHODOLOGY = 'objdiff-report'
GENERATION = 1
CODE_SECTIONS = ('.init', '.text')
ADDRESS_NAME = re.compile(r'fn_[0-9A-F]{8}')


def _tag(name):
    m = re.search(r'^config\.%s_tag = "([^"]+)"' % name, open(os.path.join(ROOT, 'configure.py')).read(), re.M)
    return m.group(1)


# The site's producer policy (config/tw2004-producer.json there) lists exactly these.
TOOLS = [{'name': 'build-container', 'identity': 'ghcr.io/mitsevox/tw2004-build:main'},
         {'name': 'decomp-toolkit', 'identity': _tag('dtk')},
         {'name': 'objdiff', 'identity': _tag('objdiff')}]


def _iso(t):
    return time.strftime('%Y-%m-%dT%H:%M:%SZ', time.gmtime(t))


def _hex(a):
    return '0x%08X' % a


def _merged(ranges):
    out = []
    for a, b in sorted(ranges):
        if out and a <= out[-1][1]:
            out[-1][1] = max(out[-1][1], b)
        else:
            out.append([a, b])
    return [{'start': _hex(a), 'end': _hex(b)} for a, b in out]


def _group(name, kind, children):
    node = {'name': name, 'kind': kind}
    for k in ('size', 'linked', 'matched') + (('fuzzy',) if kind == 'code' else ()):
        node[k] = sum(c[k] for c in children)
    node['mapped_extents'] = _merged((int(c['address'], 16), int(c['address'], 16) + c['size']) for c in children)
    node['children'] = children
    return node


def _leaves(unit):
    """A unit's function leaves (code) and section leaves (data), each as (section, leaf)."""
    linked = unit['metadata'].get('complete', False)
    code, data = [], []
    for f in unit.get('functions', []):
        size, fuzzy = int(f['size']), f.get('fuzzy_match_percent', 0.0)
        address = int(f['metadata']['virtual_address'])
        section = next(s['name'] for s in unit['sections']
                       if int(s['metadata']['virtual_address']) <= address < int(s['metadata']['virtual_address']) + int(s['size']))
        code.append((section, {'name': f['name'], 'kind': 'code', 'type': 'function', 'size': size,
                               'linked': size if linked else 0, 'matched': size if fuzzy == 100.0 else 0,
                               'fuzzy': size * fuzzy / 100.0, 'address': _hex(address),
                               'function_address': _hex(address), 'original_size': size}))
    # objdiff gives a unit's matched data bytes as one number; it lands on the sections that match.
    unmatched = int(unit['measures'].get('total_data', 0)) - int(unit['measures'].get('matched_data', 0))
    for s in unit['sections']:
        if s['name'] in CODE_SECTIONS or not int(s['size']):
            continue
        size = int(s['size'])
        whole = s.get('fuzzy_match_percent', 0.0) == 100.0
        if not whole:
            unmatched -= size
        data.append((s['name'], {'name': s['name'], 'kind': 'data', 'size': size, 'linked': size if linked else 0,
                                 'matched': size if whole else 0, 'address': _hex(int(s['metadata']['virtual_address']))}))
    if unmatched:
        raise SystemExit('site_report.py: %s: matched data does not split by section' % unit['name'])
    return code, data


def build_map(report):
    sections, files = {}, []
    for unit in report['units']:
        code, data = _leaves(unit)
        for kind, leaves in (('code', code), ('data', data)):
            for section, leaf in leaves:
                sections.setdefault((kind, section), []).append(dict(leaf, name=unit['metadata']['source_path'] + ': ' + leaf['name']) if kind == 'data' else leaf)
            if leaves:
                files.append(_group(unit['metadata']['source_path'], kind, [leaf for _, leaf in leaves]))
    order = sorted(sections, key=lambda k: min(int(c['address'], 16) for c in sections[k]))
    return {'schemaVersion': 1, 'sections': [_group(name, kind, sorted(sections[(kind, name)], key=lambda c: c['address']))
                                             for kind, name in order], 'files': files}


def build_summary(report, map_doc, audit_path):
    m = report['measures']
    code = [n for n in map_doc['files'] if n['kind'] == 'code']
    functions = [c for n in code for c in n['children']]
    summary = {'code': {'totalBytes': int(m['total_code']), 'linkedBytes': int(m.get('complete_code', 0)),
                        'matchedBytes': int(m.get('matched_code', 0)),
                        'fuzzyEquivalentBytes': sum(n['fuzzy'] for n in code)},
               'data': {'totalBytes': int(m['total_data']), 'linkedBytes': int(m.get('complete_data', 0)),
                        'matchedBytes': int(m.get('matched_data', 0)), 'fuzzyEquivalentBytes': None},
               'functions': {'total': len(functions), 'exact': sum(f['matched'] == f['size'] for f in functions),
                             'named': sum(not ADDRESS_NAME.match(f['name']) for f in functions),
                             'basis': 'functions-without-address-names'}}
    if summary['functions']['total'] != int(m['total_functions']) or summary['functions']['exact'] != int(m['matched_functions']):
        raise SystemExit('site_report.py: function inventory differs from report.json')
    done = {row.split('\t')[0] for row in open(audit_path).read().splitlines()[1:] if row.split('\t')[1] == 'done'}
    game = [u['metadata']['source_path'][len('src/'):] for u in report['units'] if 'game' in u['metadata']['progress_categories']]
    summary['files'] = {'total': len(game), 'exact': sum(p in done for p in game),
                        'named': sum(not re.fullmatch(r'Code[0-9A-F]{8}\.c', os.path.basename(p)) for p in game),
                        'basis': 'ea-game-files-audited'}
    return summary


def build_timeline(history_path, summary):
    commits = {}
    for c in subprocess.run(['git', 'rev-list', 'HEAD'], cwd=ROOT, capture_output=True, text=True,
                            check=True).stdout.split():
        commits.setdefault(c[:7], []).append(c)
    total = {k: summary[k]['totalBytes'] for k in ('code', 'data')}
    snapshots = []
    for r in json.load(open(history_path)):
        sha = r.get('sha') or commits.get(r['commit'], [])
        if isinstance(sha, list):
            if len(sha) != 1:
                raise SystemExit('site_report.py: history commit %s is not one commit of main' % r['commit'])
            sha = sha[0]
        if 'total_code' in r:
            row = {'code': {'total': r['total_code'], 'linked': r['complete_code']},
                   'data': {'total': r['total_data'], 'linked': r['complete_data']},
                   'provenance': {'kind': 'inherited', 'source': 'tw2004 progress page history (CI build of main)'}}
        else:
            linked = ('complete_code', 'complete_data') if r.get('report') else ('linked_code', 'linked_data')
            row = {'code': {'total': total['code'], 'linked': r.get(linked[0], 0)},
                   'data': {'total': total['data'], 'linked': r.get(linked[1], 0)},
                   'provenance': {'kind': 'reconstructed',
                                  'source': 'tw2004 backfill_history.py rebuild; totals from the current report'}}
        snapshots.append({'revision': sha, 'built_at': _iso(r['time']), **row})
    return {'schemaVersion': 1, 'snapshots': snapshots}


def main(report_path, history_path, out, sha, tree, run_id, attempt, workflow_blob):
    measured = _iso(time.time())
    report = json.load(open(report_path))
    map_doc = build_map(report)
    summary = build_summary(report, map_doc, os.path.join(ROOT, 'docs', 'audit.tsv'))
    timeline = build_timeline(history_path, summary)
    public = os.path.join(out, 'public-input')
    os.makedirs(public)
    for name, doc in (('summary', summary), ('map', map_doc), ('timeline', timeline)):
        json.dump(doc, open(os.path.join(public, name + '.json'), 'w'), separators=(',', ':'))
    metadata = {'project': PROJECT, 'repository': 'mitsevox/tw2004', 'target': TARGET, 'revision': sha, 'tree': tree,
                'methodology': METHODOLOGY, 'generation': GENERATION, 'measuredAt': measured,
                'publishedAt': _iso(time.time()), 'tools': TOOLS, 'runId': int(run_id), 'attempt': int(attempt),
                'workflowBlob': workflow_blob}
    json.dump(metadata, open(os.path.join(out, 'metadata.json'), 'w'), indent=1)
    # Package against the same tracked policy that the shared site reviews.
    policy = json.load(open(os.path.join(ROOT, 'config', 'GW4E69', 'producer.json')))
    if (policy['repository'] != metadata['repository'] or policy['target'] != TARGET
            or policy['tools'] != TOOLS or workflow_blob not in policy['workflowBlobs']):
        raise SystemExit('site_report.py: tracked producer policy differs from this execution')
    json.dump(policy, open(os.path.join(out, 'policy.json'), 'w'), indent=1)
    s = summary
    print('site_report.py: %s code %d/%d, data %d/%d, functions %d exact %d named, files %d exact %d named, %d history rows'
          % (sha[:7], s['code']['matchedBytes'], s['code']['totalBytes'], s['data']['matchedBytes'], s['data']['totalBytes'],
             s['functions']['exact'], s['functions']['named'], s['files']['exact'], s['files']['named'], len(timeline['snapshots'])))


if __name__ == '__main__':
    if len(sys.argv) != 9:
        raise SystemExit(__doc__)
    main(*sys.argv[1:])
