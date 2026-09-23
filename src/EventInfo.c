// EventInfo.c (our name): the front end's panel of details about a calendar day's event: for a
// PGA TOUR event the purse, round, course, leader, score and the defending champion, for a
// real-time event its rewards, status and dates. Each function fills one line's label and value.
// TW06 keeps the like in fe_calendarpopups.c, but nothing here proves the pairing.

#include "game_types.h"

void fn_8011D658(int nLine, char* szLabel, char* szValue);

void fn_8011D858(int nLine, char* szLabel, char* szValue) {
    fn_8011D658(nLine, szLabel, szValue);
}
