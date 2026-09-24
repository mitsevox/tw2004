// *****************************************************************************
// FILE -- /usr/local/sce/ee/lib/crt0.s
// *****************************************************************************

/* 00100008 000001c0 */ _start() {}
/* 001001c8 00000008 */ _exit() {}
/* 001001d0 00000008 */ static _root() {}

// *****************************************************************************
// FILE -- cc6taaaa.c
// *****************************************************************************

/* 001001d8 00000040 */ static reg_frame() {}
/* 00100218 00000038 */ static dereg_frame() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/AptViewer/TibExt.cpp
// *****************************************************************************

/* 00100250 00000024 */ TibExtMemAlloc() {}
/* 00100278 0000001c */ TibExtMemFree() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/AptViewer/eagl_real_viewer.cpp
// *****************************************************************************

/* 00100298 00000058 */ __builtin_new() {}
/* 001002f0 00000048 */ __builtin_delete() {}
/* 00100338 00000058 */ __builtin_vec_new() {}
/* 00100390 00000048 */ __builtin_vec_delete() {}
/* 001003d8 00000054 */ myNew() {}
/* 00100430 0000001c */ myDelete() {}
/* 00100450 00000070 */ setupSND() {}
/* 001004c0 00000384 */ setupEAGL() {}
/* 00100848 000001d0 */ _saveScreenShot() {}
/* 00100a18 0000001c */ absf() {}
/* 00100a38 00000040 */ _capValue() {}
/* 00100a78 00000450 */ _getInput() {}
/* 00100ec8 00000460 */ _doInput() {}
/* 00101328 00000098 */ _checkControllers() {}
/* 001013c0 00000100 */ _addKeyboardCode() {}
/* 001014c0 000000d0 */ _PS2LoadFile() {}
/* 00101590 000000a0 */ _fixNameAndLoad() {}
/* 00101630 000001c0 */ _loadFontFiles() {}
/* 001017f0 00000184 */ _reshapeViewport() {}
/* 00101978 00000008 */ _drawFPS() {}
/* 00101980 00000054 */ _allocateMemory() {}
/* 001019d8 00000044 */ _freeMemory() {}
/* 00101a20 000000c8 */ _reallocateMemory() {}
/* 00101ae8 00000030 */ printFreeMem() {}
/* 00101b18 0000052c */ viewerMain() {}
/* 00102048 000002f0 */ viewerExtracted() {}
/* 00102338 000000e8 */ viewerShutdown() {}
/* 00102420 00000098 */ loadModule() {}
/* 001024b8 00000048 */ static _InitKbdCallback() {}
/* 00102500 0000008c */ static _ProcessCallback() {}
/* 00102590 0000006c */ static _skuInitCDROMDrive() {}
/* 00102600 00000298 */ _skuInit() {}
/* 00102898 000000e4 */ main() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/AptViewer/loading.c
// *****************************************************************************

/* 00102980 00000168 */ static _LoadingInitSplashPacket() {}
/* 00102ae8 000001d8 */ static _LoadingSplashLoadTex() {}
/* 00102cc0 0000008c */ static _LoadingSplashDraw() {}
/* 00102d50 00000044 */ static _LoadingYieldThread() {}
/* 00102d98 000000a4 */ static _LoadingThreadStart() {}
/* 00102e40 000000b8 */ static _LoadingThreadAnimate() {}
/* 00102ef8 00000124 */ static _LoadingThreadCalcTransform() {}
/* 00103020 0000001c */ static _LoadingThreadTransformModel() {}
/* 00103040 000000d8 */ static _LoadingThreadDrawFace() {}
/* 00103118 00000144 */ static _LoadingThreadDrawModel() {}
/* 00103260 00000070 */ static _LoadingThreadDisplay() {}
/* 001032d0 00000058 */ static _LoadingThreadStop() {}
/* 00103328 00000194 */ static _LoadingThread() {}
/* 001034c0 000000dc */ LoadingStart() {}
/* 001035a0 0000004c */ LoadingStop() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/AptViewer/small_object_integration.cpp
// *****************************************************************************

/* 001035f0 000000b8 */ static AptViewerAlloc() {}
/* 001036a8 000000f4 */ static AptViewerFree() {}
/* 001037a0 00000104 */ static AptViewerFreeSize() {}
/* 001038a8 0000042c */ SmallObjectInitialize() {}
/* 00103cd8 0000012c */ SmallObjectShutdown() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptArray.cpp
// *****************************************************************************

/* 00103f48 00000060 */ AptArray::AptArray() {}
/* 00103fa8 00000128 */ AptArray::~AptArray() {}
/* 001040d0 000001d4 */ AptArray::CleanNativeFunctions() {}
/* 001042a8 000000c0 */ AptArray::DestroyGCPointers() {}
/* 00104368 00000040 */ AptArray::GetNumGCPointers() {}
/* 001043a8 00000078 */ AptArray::GetGCPointer() {}
/* 00104420 000000f0 */ AptArray::_reserve() {}
/* 00104510 000000e0 */ AptArray::set() {}
/* 001045f0 00000054 */ AptArray::get() {}
/* 00104648 0000007c */ AptArray::toString() {}
/* 001046c8 00000194 */ AptArray::toString() {}
/* 00104860 00000900 */ AptArray::objectMemberLookup() {}
/* 00105160 00000038 */ static _isIndex() {}
/* 00105198 00000078 */ AptArray::objectMemberSet() {}
/* 00105210 000001cc */ AptArray::sMethod_concat() {}
/* 001053e0 00000200 */ AptArray::sMethod_join() {}
/* 001055e0 00000084 */ AptArray::sMethod_pop() {}
/* 00105668 000001b0 */ AptArray::sMethod_push() {}
/* 00105818 000000a4 */ AptArray::sMethod_shift() {}
/* 001058c0 000001f8 */ AptArray::sMethod_unshift() {}
/* 00105ab8 000000d8 */ static defaultSortCompareFunc() {}
/* 00105b90 00000170 */ static scriptFunctionSortFunc() {}
/* 00105d00 000000b0 */ AptArray::sMethod_sort() {}
/* 00105db0 0000016c */ static defaultSortOnCompareFunc() {}
/* 00105f20 000000dc */ AptArray::sMethod_sortOn() {}
/* 00106000 000000b4 */ AptArray::sMethod_reverse() {}
/* 001060b8 00000268 */ AptArray::sMethod_splice() {}
/* 00106320 0000018c */ AptArray::sMethod_slice() {}
/* 001064b0 00000070 */ static __static_initialization_and_destruction_0() {}
/* 00106520 00000020 */ _GLOBAL_$I$C__Work_dash_cfe_packages_Apt_Dash_2006_source_Apt_AptArray.cppY3aaaa() {}
/* 00106540 00000020 */ _GLOBAL_$D$C__Work_dash_cfe_packages_Apt_Dash_2006_source_Apt_AptArray.cppMXbaaa() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptArrayMembers.h
// *****************************************************************************

/* 00103e08 00000140 */ ArrayMembersIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptNativeFunction.h
// *****************************************************************************

/* ffffffff 00000078 */ AptNativeFunction::~AptNativeFunction() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_Apt.h
// *****************************************************************************

/* ffffffff 00000010 */ AptArray::ConvertAptValue() {}
/* ffffffff 00000008 */ AptArray::length() {}
/* ffffffff 00000018 */ AptArray::IsValueGC() {}
/* ffffffff 00000020 */ AptArray::GetAt() {}
/* ffffffff 000000b4 */ AptArray::SetAt() {}
/* ffffffff 0000003c */ AptArray::operator new() {}
/* ffffffff 00000044 */ AptArray::operator delete() {}
/* ffffffff 00000008 */ AptArray::operator new []() {}
/* ffffffff 00000008 */ AptArray::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptMiscObjects.cpp
// *****************************************************************************

/* 00106a30 000002ec */ AptMathObj::CleanNativeFunctions() {}
/* 00106d20 00000c14 */ AptMathObj::objectMemberLookup() {}
/* 00107938 0000014c */ AptMathObj::sMethod_sin() {}
/* 00107a88 0000014c */ AptMathObj::sMethod_cos() {}
/* 00107bd8 00000170 */ AptMathObj::sMethod_atan2() {}
/* 00107d48 00000178 */ AptMathObj::sMethod_round() {}
/* 00107ec0 0000018c */ AptMathObj::sMethod_min() {}
/* 00108050 0000018c */ AptMathObj::sMethod_max() {}
/* 001081e0 0000014c */ AptMathObj::sMethod_abs() {}
/* 00108330 0000014c */ AptMathObj::sMethod_acos() {}
/* 00108480 0000014c */ AptMathObj::sMethod_asin() {}
/* 001085d0 0000014c */ AptMathObj::sMethod_atan() {}
/* 00108720 0000014c */ AptMathObj::sMethod_ceil() {}
/* 00108870 0000014c */ AptMathObj::sMethod_exp() {}
/* 001089c0 0000014c */ AptMathObj::sMethod_floor() {}
/* 00108b10 0000014c */ AptMathObj::sMethod_log() {}
/* 00108c60 00000170 */ AptMathObj::sMethod_pow() {}
/* 00108dd0 0000014c */ AptMathObj::sMethod_random() {}
/* 00108f20 00000144 */ AptMathObj::sMethod_sqrt() {}
/* 00109068 0000014c */ AptMathObj::sMethod_tan() {}
/* 001091b8 0000010c */ AptKey::CleanNativeFunctions() {}
/* 001092c8 0000067c */ AptKey::objectMemberLookup() {}
/* 00109948 00000160 */ AptKey::sMethod_getCode() {}
/* 00109aa8 0000017c */ AptKey::sMethod_getAscii() {}
/* 00109c28 00000124 */ AptKey::sMethod_getController() {}
/* 00109d50 00000114 */ AptKey::sMethod_addListener() {}
/* 00109e68 0000037c */ AptKey::sMethod_removeListener() {}
/* 0010a1e8 000006bc */ AptKey::sMethod_getAnalogStickInfo() {}
/* 0010a8a8 0000006c */ AptMouse::CleanNativeFunctions() {}
/* 0010a918 000001b0 */ AptMouse::objectMemberLookup() {}
/* 0010aac8 00000114 */ AptMouse::sMethod_addListener() {}
/* 0010abe0 0000037c */ AptMouse::sMethod_removeListener() {}
/* 0010af60 00000038 */ AptObject::objectMemberLookup() {}
/* 0010af98 00000630 */ AptLoadVars::objectMemberLookup() {}
/* 0010b5c8 00000078 */ AptLoadVars::~AptLoadVars() {}
/* 0010b640 0000010c */ AptLoadVars::CleanNativeFunctions() {}
/* 0010b750 00000430 */ AptLoadVars::sMethod_load() {}
/* 0010bb80 000003d4 */ AptLoadVars::sMethod_send() {}
/* 0010bf58 00000628 */ AptLoadVars::sMethod_sendAndLoad() {}
/* 0010c580 0000012c */ AptLoadVars::sMethod_getBytesTotal() {}
/* 0010c6b0 0000012c */ AptLoadVars::sMethod_getBytesLoaded() {}
/* 0010c7e0 00000130 */ AptLoadVars::sMethod_toString() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptKeyMembers.h
// *****************************************************************************

/* 00106560 000001a4 */ KeyMembersIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptMathMembers.h
// *****************************************************************************

/* 00106708 000001e8 */ MathMembersIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptLoadVarsMembers.h
// *****************************************************************************

/* 001068f0 00000140 */ LoadVarsMembersIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptObject.h
// *****************************************************************************

/* ffffffff 00000050 */ AptObject::AptObject() {}
/* ffffffff 00000024 */ AptObject::setHasClass() {}
/* ffffffff 0000000c */ AptObject::getHasClass() {}
/* ffffffff 00000024 */ AptObject::setInMainInst() {}
/* ffffffff 0000000c */ AptObject::getInMainInst() {}
/* ffffffff 0000001c */ AptObject::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptObject::GetGCPointer() {}
/* ffffffff 0000001c */ AptObject::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptObject::IsGarbageCollected() {}
/* ffffffff 0000001c */ AptObject::Set() {}
/* ffffffff 0000001c */ AptObject::Lookup() {}
/* ffffffff 0000001c */ AptObject::setProto() {}
/* ffffffff 0000001c */ AptObject::setPrototype() {}
/* ffffffff 00000008 */ AptObject::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptObject::getNativeHash() {}
/* ffffffff 00000008 */ AptObject::ContainsNativeHashVirtual() {}
/* ffffffff 0000003c */ AptObject::operator new() {}
/* ffffffff 00000044 */ AptObject::operator delete() {}
/* ffffffff 00000008 */ AptObject::operator new []() {}
/* ffffffff 00000008 */ AptObject::operator delete []() {}
/* ffffffff 00000078 */ AptObject::~AptObject() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptNativeFunction.h
// *****************************************************************************

/* ffffffff 00000078 */ AptNativeFunction::~AptNativeFunction() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_Apt.h
// *****************************************************************************

/* ffffffff 0000005c */ AptMathObj::AptMathObj() {}
/* ffffffff 00000008 */ AptMathObj::AddRef() {}
/* ffffffff 00000008 */ AptMathObj::Release() {}
/* ffffffff 0000001c */ AptMathObj::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptMathObj::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptMathObj::getNativeHash() {}
/* ffffffff 00000008 */ AptMathObj::ContainsNativeHashVirtual() {}
/* ffffffff 00000008 */ AptMathObj::IsGarbageCollected() {}
/* ffffffff 0000001c */ AptMathObj::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptMathObj::GetGCPointer() {}
/* ffffffff 0000003c */ AptMathObj::operator new() {}
/* ffffffff 00000044 */ AptMathObj::operator delete() {}
/* ffffffff 00000008 */ AptMathObj::operator new []() {}
/* ffffffff 00000008 */ AptMathObj::operator delete []() {}
/* ffffffff 00000078 */ AptMathObj::~AptMathObj() {}
/* ffffffff 0000005c */ AptKey::AptKey() {}
/* ffffffff 00000008 */ AptKey::AddRef() {}
/* ffffffff 00000008 */ AptKey::Release() {}
/* ffffffff 0000001c */ AptKey::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptKey::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptKey::getNativeHash() {}
/* ffffffff 00000008 */ AptKey::ContainsNativeHashVirtual() {}
/* ffffffff 00000008 */ AptKey::IsGarbageCollected() {}
/* ffffffff 0000001c */ AptKey::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptKey::GetGCPointer() {}
/* ffffffff 0000003c */ AptKey::operator new() {}
/* ffffffff 00000044 */ AptKey::operator delete() {}
/* ffffffff 00000008 */ AptKey::operator new []() {}
/* ffffffff 00000008 */ AptKey::operator delete []() {}
/* ffffffff 00000078 */ AptKey::~AptKey() {}
/* ffffffff 0000005c */ AptMouse::AptMouse() {}
/* ffffffff 00000008 */ AptMouse::AddRef() {}
/* ffffffff 00000008 */ AptMouse::Release() {}
/* ffffffff 0000001c */ AptMouse::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptMouse::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptMouse::getNativeHash() {}
/* ffffffff 00000008 */ AptMouse::ContainsNativeHashVirtual() {}
/* ffffffff 00000008 */ AptMouse::IsGarbageCollected() {}
/* ffffffff 0000001c */ AptMouse::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptMouse::GetGCPointer() {}
/* ffffffff 0000003c */ AptMouse::operator new() {}
/* ffffffff 00000044 */ AptMouse::operator delete() {}
/* ffffffff 00000008 */ AptMouse::operator new []() {}
/* ffffffff 00000008 */ AptMouse::operator delete []() {}
/* ffffffff 00000078 */ AptMouse::~AptMouse() {}
/* ffffffff 00000058 */ AptLoadVars::AptLoadVars() {}
/* ffffffff 0000003c */ AptLoadVars::operator new() {}
/* ffffffff 00000044 */ AptLoadVars::operator delete() {}
/* ffffffff 00000008 */ AptLoadVars::operator new []() {}
/* ffffffff 00000008 */ AptLoadVars::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}
/* ffffffff 00000028 */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}
/* ffffffff 00000074 */ EAStringC::operator=() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptNativeHash.h
// *****************************************************************************

/* ffffffff 000000a8 */ AptNativeHash::SetProtoGC() {}
/* ffffffff 000000a8 */ AptNativeHash::SetPrototypeGC() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptString.cpp
// *****************************************************************************

/* 0010caf8 000000a8 */ AptString::AptString() {}
/* 0010cba0 0000008c */ AptString::~AptString() {}
/* 0010cc30 00000050 */ AptString::DeleteThis() {}
/* 0010cc80 00000050 */ AptString::ForceDelete() {}
/* 0010ccd0 000001fc */ AptString::CleanNativeFunctions() {}
/* 0010ced0 00000164 */ AptString::sMethod_charAt() {}
/* 0010d038 000001f0 */ AptString::sMethod_charCodeAt() {}
/* 0010d228 00000228 */ AptString::sMethod_concat() {}
/* 0010d450 000001e0 */ AptString::sMethod_fromCharCode() {}
/* 0010d630 00000284 */ AptString::sMethod_indexOf() {}
/* 0010d8b8 0000000c */ AptString::sMethod_lastIndexOf() {}
/* 0010d8c8 00000308 */ AptString::sMethod_slice() {}
/* 0010dbd0 00000624 */ AptString::sMethod_split() {}
/* 0010e1f8 00000250 */ AptString::sMethod_substr() {}
/* 0010e448 00000274 */ AptString::sMethod_substring() {}
/* 0010e6c0 00000158 */ AptString::sMethod_toLowerCase() {}
/* 0010e818 00000158 */ AptString::sMethod_toUpperCase() {}
/* 0010e970 00000990 */ AptString::objectMemberLookup() {}
/* 0010f300 000000dc */ AptString::printf() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptStringMembers.h
// *****************************************************************************

/* 0010c910 000001e8 */ StringMembersIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/include/Apt/AptValue/AptString.h
// *****************************************************************************

/* ffffffff 0000001c */ AptString::cpy() {}
/* ffffffff 00000040 */ AptString::cpy() {}
/* ffffffff 0000001c */ AptString::cat() {}
/* ffffffff 0000001c */ AptString::cat() {}
/* ffffffff 00000008 */ AptString::GetInternalString() {}
/* ffffffff 00000008 */ AptString::SetNext() {}
/* ffffffff 00000008 */ AptString::GetNext() {}
/* ffffffff 00000008 */ AptString::GetNumGCPointers() {}
/* ffffffff 00000008 */ AptString::GetGCPointer() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptNativeFunction.h
// *****************************************************************************

/* ffffffff 00000078 */ AptNativeFunction::~AptNativeFunction() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/include/Apt/AptValue/AptString.inl
// *****************************************************************************

/* ffffffff 000000b8 */ AptString::Create() {}
/* ffffffff 00000050 */ AptString::Destroy() {}
/* ffffffff 00000024 */ AptString::operator new() {}
/* ffffffff 00000024 */ AptString::operator delete() {}
/* ffffffff 00000024 */ AptString::operator new []() {}
/* ffffffff 00000024 */ AptString::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}
/* ffffffff 00000028 */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}
/* ffffffff 00000074 */ EAStringC::operator=() {}
/* ffffffff 00000064 */ EAStringC::Clear() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptActionInterpreter.cpp
// *****************************************************************************

/* 0010f3e0 00000320 */ _isNaN() {}
/* 0010f700 00000104 */ AptActionInterpreter::initStacks() {}
/* 0010f808 00000134 */ AptActionInterpreter::shutdownStacks() {}
/* 0010f940 00000864 */ AptActionInterpreter::_parseStream() {}
/* 001101a8 00000020 */ AptActionInterpreter::unresolveStream() {}
/* 001101c8 0000001c */ AptActionInterpreter::resolveStream() {}
/* 001101e8 00000270 */ AptActionInterpreter::loadVariables() {}
/* 00110458 0000011c */ AptActionInterpreter::getObject() {}
/* 00110578 0000016c */ AptActionInterpreter::getContext() {}
/* 001106e8 00000214 */ AptActionInterpreter::getContext() {}
/* 00110900 00000144 */ static _updateObjectMethods() {}
/* 00110a48 000003a0 */ AptActionInterpreter::setVariable() {}
/* 00110de8 00000468 */ AptActionInterpreter::cbCallMethod_setInterval() {}
/* 00111250 00000120 */ AptActionInterpreter::cbCallMethod_clearInterval() {}
/* 00111370 00000220 */ AptActionInterpreter::cbCallMethod_hitTest() {}
/* 00111590 00000174 */ AptActionInterpreter::cbCallMethod_isNaN() {}
/* 00111708 00000194 */ AptActionInterpreter::cbCallMethod_unescape() {}
/* 001118a0 0000016c */ AptActionInterpreter::cbCallMethod_escape() {}
/* 00111a10 0000053c */ AptActionInterpreter::cbCallMethod_boolean() {}
/* 00111f50 00000214 */ static _getNameString() {}
/* 00112168 00000144 */ AptActionInterpreter::getName2() {}
/* 001122b0 000000b8 */ AptActionInterpreter::getName() {}
/* 00112368 0000027c */ AptActionInterpreter::_doCloneSprite() {}
/* 001125e8 000003d0 */ AptActionInterpreter::getVariable() {}
/* 001129b8 000000c4 */ AptActionInterpreter::valueToObject() {}
/* 00112a80 00000758 */ AptActionInterpreter::callFunction() {}
/* 001131d8 00000b2c */ AptActionInterpreter::_createObject() {}
/* 00113d08 00000068 */ AptActionInterpreter::clearRegisters() {}
/* 00113d70 000002e4 */ AptActionInterpreter::_doEnumerate() {}
/* 00114058 000001e4 */ static _concatAsStrings() {}
/* 00114240 0000023c */ AptActionInterpreter::runStream() {}
/* 00114480 00000174 */ _escape() {}
/* 001145f8 00000044 */ _escape2Char() {}
/* 00114640 00000150 */ _unEscape() {}
/* 00114790 00000180 */ AptActionInterpreter::urlDecode() {}
/* 00114910 00000048 */ AptActionInterpreter::isFSCommand() {}
/* 00114958 00000050 */ AptActionInterpreter::doFSCommand() {}
/* 001149a8 00000008 */ AptActionInterpreter::_FunctionAptActionEnd() {}
/* 001149b0 0000004c */ AptActionInterpreter::_FunctionAptActionNextFrame() {}
/* 00114a00 0000004c */ AptActionInterpreter::_FunctionAptActionPrevFrame() {}
/* 00114a50 00000074 */ AptActionInterpreter::_FunctionAptActionPlay() {}
/* 00114ac8 00000078 */ AptActionInterpreter::_FunctionAptActionStop() {}
/* 00114b40 00000008 */ AptActionInterpreter::_FunctionAptActionToggleQuality() {}
/* 00114b48 00000008 */ AptActionInterpreter::_FunctionAptActionStopSounds() {}
/* 00114b50 00000214 */ AptActionInterpreter::_FunctionAptActionAdd() {}
/* 00114d68 00000214 */ AptActionInterpreter::_FunctionAptActionSubtract() {}
/* 00114f80 00000204 */ AptActionInterpreter::_FunctionAptActionMultiply() {}
/* 00115188 00000224 */ AptActionInterpreter::_FunctionAptActionDivide() {}
/* 001153b0 00000238 */ AptActionInterpreter::_FunctionAptActionEquals() {}
/* 001155e8 00000214 */ AptActionInterpreter::_FunctionAptActionLessThan() {}
/* 00115800 00000234 */ AptActionInterpreter::_FunctionAptActionAnd() {}
/* 00115a38 00000238 */ AptActionInterpreter::_FunctionAptActionOr() {}
/* 00115c70 000001dc */ AptActionInterpreter::_FunctionAptActionNot() {}
/* 00115e50 000002a8 */ AptActionInterpreter::_FunctionAptActionStringEquals() {}
/* 001160f8 00000008 */ AptActionInterpreter::_FunctionAptActionStringLength() {}
/* 00116100 00000388 */ AptActionInterpreter::_FunctionAptActionSubString() {}
/* 00116488 0000008c */ AptActionInterpreter::_FunctionAptActionPop() {}
/* 00116518 000001c0 */ AptActionInterpreter::_FunctionAptActionToInteger() {}
/* 001166d8 0000013c */ AptActionInterpreter::_FunctionAptActionGetVariable() {}
/* 00116818 00000158 */ AptActionInterpreter::_FunctionAptActionSetVariable() {}
/* 00116970 00000130 */ AptActionInterpreter::_FunctionAptActionSetTarget2() {}
/* 00116aa0 00000108 */ AptActionInterpreter::_FunctionAptActionStringAdd() {}
/* 00116ba8 00000218 */ AptActionInterpreter::_FunctionAptActionGetProperty() {}
/* 00116dc0 0000012c */ AptActionInterpreter::_FunctionAptActionSetProperty() {}
/* 00116ef0 00000104 */ AptActionInterpreter::_FunctionAptActionCloneSprite() {}
/* 00116ff8 000000dc */ AptActionInterpreter::_FunctionAptActionRemoveSprite() {}
/* 001170d8 00000150 */ AptActionInterpreter::_FunctionAptActionTrace() {}
/* 00117228 00000008 */ AptActionInterpreter::_FunctionAptActionStartDragMovie() {}
/* 00117230 00000008 */ AptActionInterpreter::_FunctionAptActionStopDragMovie() {}
/* 00117238 00000008 */ AptActionInterpreter::_FunctionAptActionStringLessThan() {}
/* 00117240 000001e8 */ AptActionInterpreter::_FunctionAptActionRandom() {}
/* 00117428 00000008 */ AptActionInterpreter::_FunctionAptActionMBLength() {}
/* 00117430 00000008 */ AptActionInterpreter::_FunctionAptActionOrd() {}
/* 00117438 0000026c */ AptActionInterpreter::_FunctionAptActionChr() {}
/* 001176a8 00000168 */ AptActionInterpreter::_FunctionAptActionGetTimer() {}
/* 00117810 00000008 */ AptActionInterpreter::_FunctionAptActionMBSubString() {}
/* 00117818 00000008 */ AptActionInterpreter::_FunctionAptActionMBOrd() {}
/* 00117820 00000008 */ AptActionInterpreter::_FunctionAptActionMBChr() {}
/* 00117828 0000027c */ AptActionInterpreter::_FunctionAptActionDelete() {}
/* 00117aa8 0000022c */ AptActionInterpreter::_FunctionAptActionDelete2() {}
/* 00117cd8 000000d4 */ AptActionInterpreter::_FunctionAptActionDefineLocal() {}
/* 00117db0 000001fc */ AptActionInterpreter::_FunctionAptActionCallFunction() {}
/* 00117fb0 0000000c */ AptActionInterpreter::_FunctionAptActionReturn() {}
/* 00117fc0 0000022c */ AptActionInterpreter::_FunctionAptActionModulo() {}
/* 001181f0 000001e8 */ AptActionInterpreter::_FunctionAptActionNewObject() {}
/* 001183d8 000001a8 */ AptActionInterpreter::_FunctionAptActionDefineLocal2() {}
/* 00118580 000001d4 */ AptActionInterpreter::_FunctionAptActionInitArray() {}
/* 00118758 000002e4 */ AptActionInterpreter::_FunctionAptActionInitObject() {}
/* 00118a40 000003c0 */ AptActionInterpreter::_FunctionAptActionTypeOf() {}
/* 00118e00 00000340 */ AptActionInterpreter::_FunctionAptActionTargetPath() {}
/* 00119140 00000020 */ AptActionInterpreter::_FunctionAptActionEnumerate() {}
/* 00119160 00000544 */ AptActionInterpreter::_FunctionAptActionAdd2() {}
/* 001196a8 000002e4 */ AptActionInterpreter::_FunctionAptActionLessThan2() {}
/* 00119990 000008b0 */ AptActionInterpreter::_FunctionAptActionEquals2() {}
/* 0011a240 00000380 */ AptActionInterpreter::_FunctionAptActionToNumber() {}
/* 0011a5c0 00000218 */ AptActionInterpreter::_FunctionAptActionToString() {}
/* 0011a7d8 00000064 */ AptActionInterpreter::_FunctionAptActionPushDuplicate() {}
/* 0011a840 00000008 */ AptActionInterpreter::_FunctionAptActionStackSwap() {}
/* 0011a848 000005e4 */ AptActionInterpreter::_FunctionAptActionGetMember() {}
/* 0011ae30 000002c0 */ AptActionInterpreter::_FunctionAptActionSetMember() {}
/* 0011b0f0 000002e4 */ AptActionInterpreter::_FunctionAptActionIncrement() {}
/* 0011b3d8 000002e4 */ AptActionInterpreter::_FunctionAptActionDecrement() {}
/* 0011b6c0 00000adc */ AptActionInterpreter::_FunctionAptActionCallMethod() {}
/* 0011c1a0 00000270 */ AptActionInterpreter::_FunctionAptActionNewMethod() {}
/* 0011c410 00000020 */ AptActionInterpreter::_FunctionAptActionEnumerate2() {}
/* 0011c430 00000214 */ AptActionInterpreter::_FunctionAptActionBitAnd() {}
/* 0011c648 00000214 */ AptActionInterpreter::_FunctionAptActionBitOr() {}
/* 0011c860 00000214 */ AptActionInterpreter::_FunctionAptActionBitXor() {}
/* 0011ca78 00000214 */ AptActionInterpreter::_FunctionAptActionBitLShift() {}
/* 0011cc90 00000214 */ AptActionInterpreter::_FunctionAptActionBitRShift() {}
/* 0011cea8 00000008 */ AptActionInterpreter::_FunctionAptActionBitURShift() {}
/* 0011ceb0 00000458 */ AptActionInterpreter::_FunctionAptActionStrictEquals() {}
/* 0011d308 000002e4 */ AptActionInterpreter::_FunctionAptActionGreater() {}
/* 0011d5f0 00000094 */ AptActionInterpreter::_FunctionAptActionGotoFrame() {}
/* 0011d688 000001b0 */ AptActionInterpreter::_FunctionAptActionGetUrl() {}
/* 0011d838 000000c4 */ AptActionInterpreter::_FunctionAptActionStoreRegister() {}
/* 0011d900 00000034 */ AptActionInterpreter::_FunctionAptActionDefineDictionary() {}
/* 0011d938 00000008 */ AptActionInterpreter::_FunctionAptActionWaitForFrame() {}
/* 0011d940 000000d4 */ AptActionInterpreter::_FunctionAptActionSetTarget() {}
/* 0011da18 00000008 */ AptActionInterpreter::_FunctionAptActionGotoLabel() {}
/* 0011da20 000000f8 */ AptActionInterpreter::_FunctionAptActionWith() {}
/* 0011db18 0000016c */ AptActionInterpreter::_FunctionAptActionPush() {}
/* 0011dc88 00000474 */ AptActionInterpreter::_FunctionAptActionGetUrl2() {}
/* 0011e100 00000218 */ AptActionInterpreter::_FunctionAptActionDefineFunction() {}
/* 0011e318 000000fc */ AptActionInterpreter::_FunctionAptActionBranchIfTrue() {}
/* 0011e418 00000180 */ AptActionInterpreter::_FunctionAptActionCallFrame() {}
/* 0011e598 00000264 */ AptActionInterpreter::_FunctionAptActionGotoFrame2() {}
/* 0011e800 00000068 */ AptActionInterpreter::_FunctionAptActionBranchAlways() {}
/* 0011e868 00000154 */ AptActionInterpreter::_FunctionAptActionPushThis() {}
/* 0011e9c0 00000154 */ AptActionInterpreter::_FunctionAptActionPushGlobal() {}
/* 0011eb18 00000158 */ AptActionInterpreter::_FunctionAptActionPush0() {}
/* 0011ec70 00000164 */ AptActionInterpreter::_FunctionAptActionPush1() {}
/* 0011edd8 00000164 */ AptActionInterpreter::_FunctionAptActionPushTrue() {}
/* 0011ef40 00000158 */ AptActionInterpreter::_FunctionAptActionPushFalse() {}
/* 0011f098 00000068 */ AptActionInterpreter::_FunctionAptActionPushNULL() {}
/* 0011f100 00000068 */ AptActionInterpreter::_FunctionAptActionPushUndefined() {}
/* 0011f168 00000094 */ AptActionInterpreter::_FunctionAptActionCallFuncAndPop() {}
/* 0011f200 00000064 */ AptActionInterpreter::_FunctionAptActionCallFuncSetVar() {}
/* 0011f268 00000094 */ AptActionInterpreter::_FunctionAptActionCallMethodPop() {}
/* 0011f300 00000064 */ AptActionInterpreter::_FunctionAptActionCallMethodSetVar() {}
/* 0011f368 0000008c */ AptActionInterpreter::_FunctionAptActionPushThisVariable() {}
/* 0011f3f8 00000068 */ AptActionInterpreter::_FunctionAptActionPushGlobalVariable() {}
/* 0011f460 00000170 */ AptActionInterpreter::_FunctionAptActionPushZeroSetVar() {}
/* 0011f5d0 000001a8 */ AptActionInterpreter::_FunctionAptActionPushString() {}
/* 0011f778 00000080 */ AptActionInterpreter::_FunctionAptActionPushStringDictByte() {}
/* 0011f7f8 00000090 */ AptActionInterpreter::_FunctionAptActionPushStringDictWord() {}
/* 0011f888 00000138 */ AptActionInterpreter::_FunctionAptActionPushStringGetVar() {}
/* 0011f9c0 000001c0 */ AptActionInterpreter::_FunctionAptActionPushStringGetMember() {}
/* 0011fb80 000001c0 */ AptActionInterpreter::_FunctionAptActionPushStringSetVar() {}
/* 0011fd40 000001c0 */ AptActionInterpreter::_FunctionAptActionPushStringSetMember() {}
/* 0011ff00 000000a8 */ AptActionInterpreter::_FunctionAptActionStringDictByteGetVar() {}
/* 0011ffa8 000000a0 */ AptActionInterpreter::_FunctionAptActionStringDictByteGetMember() {}
/* 00120048 00000108 */ AptActionInterpreter::_FunctionAptActionDictCallFuncPop() {}
/* 00120150 000000d4 */ AptActionInterpreter::_FunctionAptActionDictCallFuncSetVar() {}
/* 00120228 00000108 */ AptActionInterpreter::_FunctionAptActionDictCallMethodPop() {}
/* 00120330 000000d4 */ AptActionInterpreter::_FunctionAptActionDictCallMethodSetVar() {}
/* 00120408 000001d4 */ AptActionInterpreter::_FunctionAptActionPushFloat() {}
/* 001205e0 00000170 */ AptActionInterpreter::_FunctionAptActionPushByte() {}
/* 00120750 00000188 */ AptActionInterpreter::_FunctionAptActionPushWord() {}
/* 001208d8 000001d0 */ AptActionInterpreter::_FunctionAptActionPushDWord() {}
/* 00120aa8 000000fc */ AptActionInterpreter::_FunctionAptActionBranchIfFalse() {}
/* 00120ba8 00000098 */ static __static_initialization_and_destruction_0() {}
/* 00120c40 00000020 */ _GLOBAL_$I$C__Work_dash_cfe_packages_Apt_Dash_2006_source_Apt_AptActionInterpreter.cppg7aaaa() {}
/* 00120c60 00000020 */ _GLOBAL_$D$C__Work_dash_cfe_packages_Apt_Dash_2006_source_Apt_AptActionInterpreter.cppm4baaa() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_AptActions.h
// *****************************************************************************

/* ffffffff 0000010c */ AptActionInterpreter::~AptActionInterpreter() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptRegister.h
// *****************************************************************************

/* ffffffff 00000008 */ AptRegister::GetNumGCPointers() {}
/* ffffffff 00000008 */ AptRegister::GetGCPointer() {}
/* ffffffff 0000005c */ AptRegister::~AptRegister() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptLookup.h
// *****************************************************************************

/* ffffffff 00000008 */ AptLookup::GetNumGCPointers() {}
/* ffffffff 00000008 */ AptLookup::GetGCPointer() {}
/* ffffffff 0000005c */ AptLookup::~AptLookup() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptNativeHash.h
// *****************************************************************************

/* ffffffff 00000010 */ AptNativeHash::ConvertAptValue() {}
/* ffffffff 000000a8 */ AptNativeHash::SetProtoGC() {}
/* ffffffff 000000a8 */ AptNativeHash::SetPrototypeGC() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_AptValue.h
// *****************************************************************************

/* ffffffff 0000001c */ AptPrototype::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptPrototype::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptPrototype::ContainsNativeHashVirtual() {}
/* ffffffff 00000008 */ AptPrototype::IsGarbageCollected() {}
/* ffffffff 0000001c */ AptPrototype::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptPrototype::GetGCPointer() {}
/* ffffffff 00000078 */ AptPrototype::~AptPrototype() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptMovieClip.h
// *****************************************************************************

/* ffffffff 00000078 */ AptMovieClip::~AptMovieClip() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptFrameStack.h
// *****************************************************************************

/* ffffffff 0000001c */ AptFrameStack::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptFrameStack::IsGarbageCollected() {}
/* ffffffff 00000008 */ AptFrameStack::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptFrameStack::ContainsNativeHashVirtual() {}
/* ffffffff 0000001c */ AptFrameStack::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptFrameStack::GetGCPointer() {}
/* ffffffff 00000078 */ AptFrameStack::~AptFrameStack() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}
/* ffffffff 00000020 */ EAStringC::EAStringC() {}
/* ffffffff 00000028 */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}
/* ffffffff 00000074 */ EAStringC::operator=() {}
/* ffffffff 00000024 */ EAStringC::operator==() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptGC.cpp
// *****************************************************************************

/* 00120c80 0000003c */ AptGC::Initialize() {}
/* 00120cc0 00000060 */ AptGC::AddGCAptValue() {}
/* 00120d20 0000007c */ AptGC::RemoveGCAptValue() {}
/* 00120da0 0000003c */ AptGC::IsValueGarbageCollected() {}
/* 00120de0 0000002c */ AptGC::GetAsGCPointer() {}
/* 00120e10 0000003c */ AptGC::RegisterTraceObject() {}
/* 00120e50 00000008 */ AptGC::GetGCLinkedList() {}
/* 00120e58 00000008 */ AptGC::GetNextGCAptValue() {}
/* 00120e60 00000008 */ AptGC::GetPrevGCAptValue() {}
/* 00120e68 0000001c */ AptGC::GarbageCollectionInBetween() {}
/* 00120e88 000001ac */ AptGC::GarbageCollection() {}
/* 00121038 000000b0 */ AptGC::RecursiveMark() {}
/* 001210e8 0000009c */ AptGC::RecursiveMark() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptAnimation.cpp
// *****************************************************************************

/* 00121188 0000052c */ AptCharacterAnimation::_fixup() {}
/* 001216b8 00000054 */ AptCharacterAnimation::resolve() {}
/* 00121710 00000038 */ AptCharacterAnimation::_unmapCharacter() {}
/* 00121748 00000040 */ AptCharacterAnimation::_isImport() {}
/* 00121788 00000778 */ AptCharacterAnimation::unresolve() {}
/* 00121f00 00000094 */ AptCharacterAnimationInst::AptCharacterAnimationInst() {}
/* 00121f98 00000080 */ AptCharacterAnimationInst::~AptCharacterAnimationInst() {}
/* 00122018 0000001c */ AptCharacterAnimationInst::PreDestroy() {}
/* 00122038 00000338 */ AptCharacterAnimation::link() {}
/* 00122370 00000200 */ AptAnimationPoolData::AptAnimationPoolData() {}
/* 00122570 000002ac */ AptAnimationPoolData::~AptAnimationPoolData() {}
/* 00122820 000002a0 */ AptAnimationPoolData::PreDestroy() {}
/* 00122ac0 00000490 */ AptAnimationPoolData::tickIntervalTimers() {}
/* 00122f50 00000164 */ AptAnimationPoolData::_tickNewInsts() {}
/* 001230b8 00000298 */ AptAnimationPoolData::runActions() {}
/* 00123350 000000a0 */ AptAnimationPoolData::addAnalogInput() {}
/* 001233f0 00000078 */ AptAnimationPoolData::addInput() {}
/* 00123468 00000038 */ AptAnimationPoolData::addInput() {}
/* 001234a0 00000038 */ AptAnimationPoolData::addInput() {}
/* 001234d8 00000184 */ AptAnimationPoolData::removeTimerFunctions() {}
/* 00123660 000000ec */ AptAnimationPoolData::clearActions() {}
/* 00123750 000000a4 */ AptAnimationPoolData::addActionBack() {}
/* 001237f8 000000a4 */ AptAnimationPoolData::addActionFront() {}
/* 001238a0 000000d0 */ AptAnimationPoolData::addFunctionBack() {}
/* 00123970 000000cc */ AptAnimationPoolData::addFunctionFront() {}
/* 00123a40 000001a0 */ AptAnimationPoolData::removeActionFor() {}
/* 00123be0 00000080 */ AptAnimationPoolData::clearBIL() {}
/* 00123c60 000000ac */ AptAnimationPoolData::appendButtonToBIL() {}
/* 00123d10 000000dc */ AptAnimationPoolData::removeFromBIL() {}
/* 00123df0 00000008 */ AptAnimationPoolData::setInputMask() {}
/* 00123df8 0000003c */ AptAnimationPoolData::isInputMasked() {}
/* 00123e38 00000190 */ AptCharacterAnimation::doImportedInitActions() {}
/* 00123fc8 0000003c */ AptAnimationPoolData::GetDequeSize() {}
/* 00124008 00000058 */ AptAnimationPoolData::GetDequeLocation() {}
/* 00124060 000000b4 */ AptAnimationPoolData::GetNumGCPointers() {}
/* 00124118 0000025c */ AptAnimationPoolData::GetGCPointer() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_Apt.h
// *****************************************************************************

/* ffffffff 0000007c */ AptIntervalTimer::cleanParams() {}
/* ffffffff 0000003c */ AptCharacterInst::DestroyGCPointers() {}
/* ffffffff 00000024 */ AptCharacterAnimationInst::operator new() {}
/* ffffffff 00000024 */ AptCharacterAnimationInst::operator delete() {}
/* ffffffff 00000024 */ AptCharacterAnimationInst::operator new []() {}
/* ffffffff 00000024 */ AptCharacterAnimationInst::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptLoad.cpp
// *****************************************************************************

/* 00124378 000000c8 */ AptLoader::findFile() {}
/* 00124440 00000078 */ AptLoader::~AptLoader() {}
/* 001244b8 000000dc */ AptLoader::Invalidate() {}
/* 00124598 00000060 */ AptLoader::notify() {}
/* 001245f8 000000a0 */ AptLoader::IsLoaded() {}
/* 00124698 00000154 */ AptLoader::Load() {}
/* 001247f0 00000134 */ AptLoader::AllImportsAvailable() {}
/* 00124928 000001b4 */ AptLoader::Update() {}
/* 00124ae0 000000d4 */ AptLoader::CompleteLoad() {}
/* 00124bb8 00000364 */ AptLoader::CancelPreloadedAnimation() {}
/* 00124f20 000009b4 */ AptLinker::Update() {}
/* 001258d8 00000674 */ AptLinker::Notify() {}
/* 00125f50 000005a8 */ AptLinker::Load() {}
/* 001264f8 00000134 */ AptLinker::CancelLoad() {}
/* 00126630 00000010 */ AptSharedPtrIncRef() {}
/* 00126640 00000010 */ AptSharedPtrDecRef() {}
/* 00126650 00000080 */ AptSharedPtrDelete() {}
/* 001266d0 00000010 */ AptSharedPtrIncRef() {}
/* 001266e0 00000010 */ AptSharedPtrDecRef() {}
/* 001266f0 0000005c */ AptSharedPtrDelete() {}
/* 00126750 00000580 */ GlobalNotificationFunction() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_AptLoad.h
// *****************************************************************************

/* ffffffff 00000008 */ AptFile::GetMainCharacter() {}
/* ffffffff 00000024 */ AptFile::operator delete() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000003e0 */ void EA::String::BasicString<StringAsVectorEncoding<AptFileSavedInputState>, StringAsVectorPolicy>::Insert<AptFileSavedInputState *>() {}
/* ffffffff 00000574 */ void EA::String::BasicString<StringAsVectorEncoding<AptSharedPtr<AptFile> >, StringAsVectorPolicy>::Insert<AptSharedPtr<AptFile> *>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000020 */ EAStringC::EAStringC() {}
/* ffffffff 00000028 */ EAStringC::EAStringC() {}
/* ffffffff 0000001c */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}
/* ffffffff 00000074 */ EAStringC::operator=() {}
/* ffffffff 00000048 */ EAStringC::operator==() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptCIH.cpp
// *****************************************************************************

/* 00126cd0 000000c4 */ AptCIH::~AptCIH() {}
/* 00126d98 000000b8 */ AptCIH::Remove() {}
/* 00126e50 00000034 */ AptCIH::PreDestroy() {}
/* 00126e88 00000064 */ AptCIH::DestroyGCPointers() {}
/* 00126ef0 000004a4 */ AptCIH::ClearCIH() {}
/* 00127398 000000d8 */ AptCIH::GetNumGCPointers() {}
/* 00127470 000000a8 */ AptCIH::GetGCPointer() {}
/* 00127518 00000070 */ AptCIH::getNativeHash() {}
/* 00127588 0000001c */ AptCIH::getNativeHashVirtual() {}
/* 001275a8 00000020 */ AptCIH::ContainsNativeHashVirtual() {}
/* 001275c8 00000020 */ AptCIH::objectMemberLookup() {}
/* 001275e8 00000024 */ AptCIH::objectMemberSet() {}
/* 00127610 0000002c */ AptCIH::AddRef() {}
/* 00127640 00000058 */ AptCIH::Release() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptCIH.h
// *****************************************************************************

/* ffffffff 00000024 */ AptCIH::setHasClass() {}
/* ffffffff 0000000c */ AptCIH::getHasClass() {}
/* ffffffff 00000024 */ AptCIH::setInMainInst() {}
/* ffffffff 0000000c */ AptCIH::getInMainInst() {}
/* ffffffff 000000d0 */ AptCIH::AptCIH() {}
/* ffffffff 00000008 */ AptCIH::getSpriteInst() {}
/* ffffffff 00000008 */ AptCIH::getSpriteInstBase() {}
/* ffffffff 00000008 */ AptCIH::getTextInst() {}
/* ffffffff 00000008 */ AptCIH::getStaticTextInst() {}
/* ffffffff 00000008 */ AptCIH::getMorphInst() {}
/* ffffffff 00000008 */ AptCIH::getButtonInst() {}
/* ffffffff 00000008 */ AptCIH::getAnimationInst() {}
/* ffffffff 00000008 */ AptCIH::getShapeInst() {}
/* ffffffff 00000008 */ AptCIH::getCharacterInst() {}
/* ffffffff 00000064 */ AptCIH::isSpriteInst() {}
/* ffffffff 0000005c */ AptCIH::isCharacterInst() {}
/* ffffffff 00000064 */ AptCIH::isButtonInst() {}
/* ffffffff 00000048 */ AptCIH::isShapeInst() {}
/* ffffffff 00000048 */ AptCIH::isTextInst() {}
/* ffffffff 00000048 */ AptCIH::isStaticTextInst() {}
/* ffffffff 00000048 */ AptCIH::isMorphInst() {}
/* ffffffff 00000064 */ AptCIH::isAnimationInst() {}
/* ffffffff 00000048 */ AptCIH::isLevelInst() {}
/* ffffffff 000000b0 */ AptCIH::isSpriteInstBase() {}
/* ffffffff 0000003c */ AptCIH::operator new() {}
/* ffffffff 00000044 */ AptCIH::operator delete() {}
/* ffffffff 00000008 */ AptCIH::operator new []() {}
/* ffffffff 00000008 */ AptCIH::operator delete []() {}
/* ffffffff 00000008 */ AptCIH::IsGarbageCollected() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000020 */ EAStringC::EAStringC() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptTextFormat.cpp
// *****************************************************************************

/* 00127808 000004a0 */ AptTextFormat::objectMemberLookup() {}
/* 00127ca8 000001c8 */ AptTextFormat::objectMemberSet() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptTextFormatMembers.h
// *****************************************************************************

/* 00127698 00000170 */ TextFormatMembersIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_Apt.h
// *****************************************************************************

/* ffffffff 00000184 */ AptTextFormat::AptTextFormat() {}
/* ffffffff 000000a4 */ AptTextFormat::AptTextFormat() {}
/* ffffffff 0000003c */ AptTextFormat::operator new() {}
/* ffffffff 00000044 */ AptTextFormat::operator delete() {}
/* ffffffff 00000008 */ AptTextFormat::operator new []() {}
/* ffffffff 00000008 */ AptTextFormat::operator delete []() {}
/* ffffffff 0000009c */ AptTextFormat::~AptTextFormat() {}
/* ffffffff 00000048 */ TextFormat::copyTextFormatObj() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}
/* ffffffff 00000028 */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}
/* ffffffff 00000074 */ EAStringC::operator=() {}
/* ffffffff 00000020 */ EAStringC::EAStringC() {}
/* ffffffff 00000024 */ EAStringC::operator==() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptDate.cpp
// *****************************************************************************

/* 00128058 000005e4 */ AptDate::CleanNativeFunctions() {}
/* 00128640 00000048 */ AptDate::dateIsYearLeap() {}
/* 00128688 0000002c */ AptDate::dateGetNumDaysInYear() {}
/* 001286b8 0000006c */ AptDate::dateGetNumDaysInMonth() {}
/* 00128728 00000128 */ AptDate::setDates() {}
/* 00128850 0000025c */ AptDate::getDayOfWeek() {}
/* 00128ab0 00000318 */ AptDate::toString() {}
/* 00128dc8 00000114 */ AptDate::sMethod_getDate() {}
/* 00128ee0 00000124 */ AptDate::sMethod_getDay() {}
/* 00129008 00000114 */ AptDate::sMethod_getFullYear() {}
/* 00129120 00000114 */ AptDate::sMethod_getHours() {}
/* 00129238 00000114 */ AptDate::sMethod_getMilliseconds() {}
/* 00129350 00000114 */ AptDate::sMethod_getMinutes() {}
/* 00129468 00000114 */ AptDate::sMethod_getMonth() {}
/* 00129580 00000114 */ AptDate::sMethod_getSeconds() {}
/* 00129698 00000108 */ AptDate::sMethod_getTime() {}
/* 001297a0 00000124 */ AptDate::sMethod_getTimezoneOffset() {}
/* 001298c8 00000114 */ AptDate::sMethod_getUTCDate() {}
/* 001299e0 00000124 */ AptDate::sMethod_getUTCDay() {}
/* 00129b08 00000114 */ AptDate::sMethod_getUTCFullYear() {}
/* 00129c20 00000114 */ AptDate::sMethod_getUTCHours() {}
/* 00129d38 00000114 */ AptDate::sMethod_getUTCMilliseconds() {}
/* 00129e50 00000114 */ AptDate::sMethod_getUTCMinutes() {}
/* 00129f68 00000114 */ AptDate::sMethod_getUTCMonth() {}
/* 0012a080 00000114 */ AptDate::sMethod_getUTCSeconds() {}
/* 0012a198 00000118 */ AptDate::sMethod_getYear() {}
/* 0012a2b0 00000154 */ AptDate::sMethod_setDate() {}
/* 0012a408 000001c8 */ AptDate::sMethod_setFullYear() {}
/* 0012a5d0 00000154 */ AptDate::sMethod_setHours() {}
/* 0012a728 00000154 */ AptDate::sMethod_setMilliseconds() {}
/* 0012a880 00000154 */ AptDate::sMethod_setMinutes() {}
/* 0012a9d8 00000154 */ AptDate::sMethod_setMonth() {}
/* 0012ab30 00000154 */ AptDate::sMethod_setSeconds() {}
/* 0012ac88 00000108 */ AptDate::sMethod_setTime() {}
/* 0012ad90 00000158 */ AptDate::sMethod_setUTCDate() {}
/* 0012aee8 000001cc */ AptDate::sMethod_setUTCFullYear() {}
/* 0012b0b8 00000158 */ AptDate::sMethod_setUTCHours() {}
/* 0012b210 00000158 */ AptDate::sMethod_setUTCMilliseconds() {}
/* 0012b368 00000158 */ AptDate::sMethod_setUTCMinutes() {}
/* 0012b4c0 00000158 */ AptDate::sMethod_setUTCMonth() {}
/* 0012b618 00000158 */ AptDate::sMethod_setUTCSeconds() {}
/* 0012b770 00000154 */ AptDate::sMethod_setYear() {}
/* 0012b8c8 00000148 */ AptDate::sMethod_toString() {}
/* 0012ba10 0000000c */ AptDate::sMethod_UTC() {}
/* 0012ba20 00001840 */ AptDate::objectMemberLookup() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptDateMembers.h
// *****************************************************************************

/* 00127e70 000001e8 */ DateMembersIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptNativeFunction.h
// *****************************************************************************

/* ffffffff 00000078 */ AptNativeFunction::~AptNativeFunction() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_Apt.h
// *****************************************************************************

/* ffffffff 000001f4 */ AptDate::AptDate() {}
/* ffffffff 0000003c */ AptDate::operator new() {}
/* ffffffff 00000044 */ AptDate::operator delete() {}
/* ffffffff 00000008 */ AptDate::operator new []() {}
/* ffffffff 00000008 */ AptDate::operator delete []() {}
/* ffffffff 00000078 */ AptDate::~AptDate() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}
/* ffffffff 00000028 */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}
/* ffffffff 00000074 */ EAStringC::operator=() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptCharacter.cpp
// *****************************************************************************

/* 0012d3c0 0000008c */ AptCharacter::render() {}
/* 0012d450 000000b0 */ AptCharacter::_getBoundingRect() {}
/* 0012d500 00000234 */ AptCharacter::GetNumGCPointers() {}
/* 0012d738 00000318 */ AptCharacter::GetGCPointer() {}
/* 0012da50 00000190 */ AptCharacterSpriteInstBase::updateObjectMethods() {}
/* 0012dbe0 00000104 */ AptCharacterSpriteInstBase::AptCharacterSpriteInstBase() {}
/* 0012dce8 000000b0 */ AptCharacterSpriteInstBase::~AptCharacterSpriteInstBase() {}
/* 0012dd98 0000001c */ AptCharacterSpriteInstBase::PreDestroy() {}
/* 0012ddb8 0000030c */ AptCharacterTextInst::SetText() {}
/* 0012e0c8 00000298 */ AptCharacterTextInst::UpdateText() {}
/* 0012e360 000002b4 */ AptCIH::ensureStringAllocated() {}
/* 0012e618 000000fc */ AptCIH::deallocAssetStringRecursive() {}
/* 0012e718 000006d8 */ AptCIH::render() {}
/* 0012edf0 000001d8 */ AptCIH::_getBoundingRect() {}
/* 0012efc8 00000050 */ AptCIH::getBoundingRect() {}
/* 0012f018 000000c4 */ AptCIH::getGlobalBoundingRect() {}
/* 0012f0e0 000000a8 */ AptCIH::getGlobalTranslation() {}
/* 0012f188 00000070 */ static fnorm_angle() {}
/* 0012f1f8 00000208 */ AptCIH::setupProceduralProperty() {}
/* 0012f400 00000014 */ AptCIH::getProceduralProperty() {}
/* 0012f418 00000120 */ AptCIH::setProceduralProperty() {}
/* 0012f538 0000010c */ AptCharacterInst::_gotoAndX() {}
/* 0012f648 000003dc */ AptCharacterInst::CleanNativeFunctions() {}
/* 0012fa28 0000001c */ AptCharacterInst::sMethod_gotoAndStop() {}
/* 0012fa48 0000001c */ AptCharacterInst::sMethod_gotoAndPlay() {}
/* 0012fa68 000002bc */ AptCharacterInst::sMethod_attachMovie() {}
/* 0012fd28 000001f0 */ AptCharacterInst::sMethod_loadMovie() {}
/* 0012ff18 000000a4 */ AptCharacterInst::sMethod_unloadMovie() {}
/* 0012ffc0 000000b8 */ AptCharacterInst::sMethod_duplicateMovieClip() {}
/* 00130078 00000078 */ AptCharacterInst::sMethod_removeMovieClip() {}
/* 001300f0 00000078 */ AptCharacterInst::sMethod_removeTextField() {}
/* 00130168 000003dc */ AptCharacterInst::sMethod_createTextField() {}
/* 00130548 0000014c */ AptCharacterInst::sMethod_getDepth() {}
/* 00130698 0000029c */ AptCharacterInst::sMethod_swapDepths() {}
/* 00130938 000000ec */ AptCharacterInst::sMethod_setMask() {}
/* 00130a28 0000050c */ AptCharacterInst::sMethod_getBounds() {}
/* 00130f38 0000052c */ AptCharacterInst::sMethod_hitTest() {}
/* 00131468 000001c8 */ AptCharacterInst::sMethod_createEmptyMovieClip() {}
/* 00131630 000000b0 */ AptCharacterInst::sMethod_loadVariables() {}
/* 001316e0 000000ac */ AptCharacterInst::sMethod_stop() {}
/* 00131790 000000a4 */ AptCharacterInst::sMethod_play() {}
/* 00131838 00000050 */ AptCharacterInst::sMethod_nextFrame() {}
/* 00131888 00000050 */ AptCharacterInst::sMethod_prevFrame() {}
/* 001318d8 0000020c */ AptCharacterInst::sMethod_getBytesTotal() {}
/* 00131ae8 0000020c */ AptCharacterInst::sMethod_getBytesLoaded() {}
/* 00131cf8 00000b8c */ AptCharacterInst::objectMemberSet() {}
/* 00132888 00002234 */ AptCharacterInst::objectMemberLookup() {}
/* 00134ac0 0000046c */ AptCIH::associateInstToClass() {}
/* 00134f30 0000022c */ AptCharacterInst::sMethod_setTextFormat() {}
/* 00135160 00000348 */ AptCharacterInst::sMethod_getNewTextFormat() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptTextMembers.h
// *****************************************************************************

/* 0012d260 00000160 */ TextMembersIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptNativeHash.h
// *****************************************************************************

/* ffffffff 00000010 */ AptNativeHash::ConvertAptValue() {}
/* ffffffff 000000a8 */ AptNativeHash::SetProtoGC() {}
/* ffffffff 000000a8 */ AptNativeHash::SetPrototypeGC() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_AptValue.h
// *****************************************************************************

/* ffffffff 0000001c */ AptPrototype::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptPrototype::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptPrototype::ContainsNativeHashVirtual() {}
/* ffffffff 00000008 */ AptPrototype::IsGarbageCollected() {}
/* ffffffff 0000001c */ AptPrototype::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptPrototype::GetGCPointer() {}
/* ffffffff 00000078 */ AptPrototype::~AptPrototype() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptNativeFunction.h
// *****************************************************************************

/* ffffffff 00000078 */ AptNativeFunction::~AptNativeFunction() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_Apt.h
// *****************************************************************************

/* ffffffff 00000048 */ TextFormat::copyTextFormatObj() {}
/* ffffffff 00000008 */ AptCharacterInst::PreDestroy() {}
/* ffffffff 0000007c */ AptCharacterInst::~AptCharacterInst() {}
/* ffffffff 0000003c */ AptCharacterInst::DestroyGCPointers() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}
/* ffffffff 00000020 */ EAStringC::EAStringC() {}
/* ffffffff 00000028 */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}
/* ffffffff 00000074 */ EAStringC::operator=() {}
/* ffffffff 00000024 */ EAStringC::operator==() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/StringPool.cpp
// *****************************************************************************

/* 001354a8 000058c8 */ StringPool::Initialize() {}
/* 0013ad70 0000014c */ StringPool::Teardown() {}
/* 0013aec0 00000088 */ StringPool::GetString() {}
/* 0013af48 00000288 */ StringPool::GetFromPool() {}
/* 0013b1d0 000000e0 */ StringPool::RemoveFromPool() {}
/* 0013b2b0 00000084 */ StringPool::ClearTemporaryPool() {}
/* 0013b338 000000b0 */ StringPool::CheckContent() {}
/* 0013b3e8 000000c0 */ static __static_initialization_and_destruction_0() {}
/* 0013b4a8 00000020 */ _GLOBAL_$I$C__Work_dash_cfe_packages_Apt_Dash_2006_source_Apt_string_StringPool.cpprJaaaa() {}
/* 0013b4c8 00000020 */ _GLOBAL_$D$C__Work_dash_cfe_packages_Apt_Dash_2006_source_Apt_string_StringPool.cppIibaaa() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptFloat.cpp
// *****************************************************************************

/* 0013b4e8 00000084 */ AptFloat::ClearPool() {}
/* 0013b570 00000014 */ AptFloat::DeleteThis() {}
/* 0013b588 00000014 */ AptFloat::ForceDelete() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/include/Apt/AptValue/AptFloat.h
// *****************************************************************************

/* ffffffff 00000008 */ AptFloat::GetNumGCPointers() {}
/* ffffffff 00000008 */ AptFloat::GetGCPointer() {}
/* ffffffff 00000054 */ AptFloat::~AptFloat() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptFloat.inl
// *****************************************************************************

/* ffffffff 0000009c */ AptFloat::AptFloat() {}
/* ffffffff 00000110 */ AptFloat::Create() {}
/* ffffffff 00000014 */ AptFloat::Destroy() {}
/* ffffffff 00000008 */ AptFloat::GetFloat() {}
/* ffffffff 00000024 */ AptFloat::operator new() {}
/* ffffffff 00000024 */ AptFloat::operator delete() {}
/* ffffffff 00000024 */ AptFloat::operator new []() {}
/* ffffffff 00000024 */ AptFloat::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptGlobalObject.cpp
// *****************************************************************************

/* 0013b5a0 00000064 */ AptGlobal::objectMemberLookup() {}
/* 0013b608 00000068 */ AptGlobal::objectMemberSet() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptGlobalObject.h
// *****************************************************************************

/* ffffffff 0000005c */ AptGlobal::AptGlobal() {}
/* ffffffff 00000008 */ AptGlobal::AddRef() {}
/* ffffffff 00000008 */ AptGlobal::Release() {}
/* ffffffff 0000001c */ AptGlobal::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptGlobal::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptGlobal::getNativeHash() {}
/* ffffffff 00000008 */ AptGlobal::ContainsNativeHashVirtual() {}
/* ffffffff 00000008 */ AptGlobal::IsGarbageCollected() {}
/* ffffffff 0000001c */ AptGlobal::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptGlobal::GetGCPointer() {}
/* ffffffff 0000003c */ AptGlobal::operator new() {}
/* ffffffff 00000044 */ AptGlobal::operator delete() {}
/* ffffffff 00000008 */ AptGlobal::operator new []() {}
/* ffffffff 00000008 */ AptGlobal::operator delete []() {}
/* ffffffff 00000078 */ AptGlobal::~AptGlobal() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptGlobalObject.inl
// *****************************************************************************

/* ffffffff 0000001c */ AptGlobal::Set() {}
/* ffffffff 0000001c */ AptGlobal::Lookup() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptScriptColour.cpp
// *****************************************************************************

/* 0013b670 00000148 */ AptScriptColour::AptScriptColour() {}
/* 0013b7b8 000000a0 */ AptScriptColour::~AptScriptColour() {}
/* 0013b858 000000bc */ AptScriptColour::CleanNativeFunctions() {}
/* 0013b918 00000370 */ AptScriptColour::objectMemberLookup() {}
/* 0013bc88 000000d8 */ AptScriptColour::sMethod_setRGB() {}
/* 0013bd60 00000170 */ AptScriptColour::sMethod_getRGB() {}
/* 0013bed0 000008fc */ AptScriptColour::sMethod_getTransform() {}
/* 0013c7d0 00000254 */ AptScriptColour::sMethod_setTransform() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptNativeFunction.h
// *****************************************************************************

/* ffffffff 00000078 */ AptNativeFunction::~AptNativeFunction() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptScriptColour.h
// *****************************************************************************

/* ffffffff 0000003c */ AptScriptColour::operator new() {}
/* ffffffff 00000044 */ AptScriptColour::operator delete() {}
/* ffffffff 00000008 */ AptScriptColour::operator new []() {}
/* ffffffff 00000008 */ AptScriptColour::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptCharacterInstButton.cpp
// *****************************************************************************

/* 0013ca28 0000001c */ AptCharacterButtonInst::PreDestroy() {}
/* 0013ca48 000000e0 */ AptCIH::gotoState() {}
/* 0013cb28 00000170 */ AptCharacterButtonInst::updateObjectMethods() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_Apt.h
// *****************************************************************************

/* ffffffff 0000003c */ AptCharacterInst::DestroyGCPointers() {}
/* ffffffff 00000090 */ AptCharacterButtonInst::~AptCharacterButtonInst() {}
/* ffffffff 00000050 */ AptCharacterButtonInst::AptCharacterButtonInst() {}
/* ffffffff 00000024 */ AptCharacterButtonInst::operator new() {}
/* ffffffff 00000024 */ AptCharacterButtonInst::operator delete() {}
/* ffffffff 00000024 */ AptCharacterButtonInst::operator new []() {}
/* ffffffff 00000024 */ AptCharacterButtonInst::operator delete []() {}
/* ffffffff 00000008 */ AptCharacterInst::PreDestroy() {}
/* ffffffff 0000007c */ AptCharacterInst::~AptCharacterInst() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptNativeHash.cpp
// *****************************************************************************

/* 0013cc98 00000018 */ AptNativeHash::AptNativeHash() {}
/* 0013ccb0 00000044 */ AptNativeHash::~AptNativeHash() {}
/* 0013ccf8 00000144 */ AptNativeHash::DestroyGCPointers() {}
/* 0013ce40 00000054 */ AptNativeHash::SetIfNotExists() {}
/* 0013ce98 000001d0 */ AptNativeHash::Set() {}
/* 0013d068 000001b4 */ AptNativeHash::Unset() {}
/* 0013d220 000000dc */ AptNativeHash::Lookup() {}
/* 0013d300 0000017c */ AptNativeHash::ClearData() {}
/* 0013d480 0000001c */ AptNativeHash::ClearAll() {}
/* 0013d4a0 0000007c */ AptNativeHash::getFirstItem() {}
/* 0013d520 00000088 */ AptNativeHash::getNextItem() {}
/* 0013d5a8 00000104 */ AptNativeHash::Expand() {}
/* 0013d6b0 0000018c */ AptNativeHash::CheckConsistency() {}
/* 0013d840 000000cc */ AptNativeHash::SlowFindKey() {}
/* 0013d910 0000001c */ AptNativeHash::GetNumGCPointers() {}
/* 0013d930 00000084 */ AptNativeHash::GetGCPointer() {}
/* ffffffff 0000000c */ AptNativeHash::IsEmpty() {}
/* ffffffff 0000004c */ AptNativeHash::FirstAllocation() {}
/* ffffffff 00000554 */ AptNativeHash::HashSet() {}
/* ffffffff 00000240 */ AptNativeHash::HashFindKey() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptCharacterInstSprite.cpp
// *****************************************************************************

/* 0013d9b8 00000294 */ AptCIH::jumpToFrame() {}
/* 0013dc50 00000778 */ AptCIH::queueClipEvents() {}
/* 0013e3c8 0000023c */ AptCIH::tick() {}
/* 0013e608 0000009c */ AptCIH::doInitActions() {}
/* 0013e6a8 000000ac */ AptCIH::checkIfHigher() {}
/* 0013e758 00000034 */ AptCIH::getParentCount() {}
/* 0013e790 0000006c */ AptCIH::getDepthOfParentAt() {}
/* 0013e800 0000004c */ AptCIH::isVisiable() {}
/* 0013e850 000000d8 */ AptCIH::isMyEvent() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptNativeHash.h
// *****************************************************************************

/* ffffffff 00000010 */ AptNativeHash::ConvertAptValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptFrameStack.h
// *****************************************************************************

/* ffffffff 0000001c */ AptFrameStack::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptFrameStack::IsGarbageCollected() {}
/* ffffffff 00000008 */ AptFrameStack::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptFrameStack::ContainsNativeHashVirtual() {}
/* ffffffff 0000001c */ AptFrameStack::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptFrameStack::GetGCPointer() {}
/* ffffffff 00000078 */ AptFrameStack::~AptFrameStack() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000020 */ EAStringC::EAStringC() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.cpp
// *****************************************************************************

/* 0013e928 00000040 */ AptValueVector::AptValueVector() {}
/* 0013e968 00000064 */ AptValueVector::~AptValueVector() {}
/* 0013e9d0 000000d8 */ AptValueVector::ReleaseValues() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/Apt.cpp
// *****************************************************************************

/* 0013eaa8 00000008 */ _profileInit() {}
/* 0013eab0 00000024 */ static _passthruToMemFree() {}
/* 0013ead8 000000a8 */ AptUserFunctions::AptUserFunctions() {}
/* 0013eb80 0000025c */ AptInitialize() {}
/* 0013ede0 0000014c */ AptShutdown() {}
/* 0013ef30 0000001c */ AptInitializeGC() {}
/* 0013ef50 00000030 */ AptInitializeGCTraceObjects() {}
/* 0013ef80 00000074 */ CleanAllNativeFunctions() {}
/* 0013eff8 00000098 */ AptCompleteAnimationAsyncLoad() {}
/* 0013f090 0000014c */ _AptGetAnimationAtLevel() {}
/* 0013f1e0 000000d4 */ AptGetAnimationSize() {}
/* 0013f2b8 00000024 */ AptSetValidFocusButton() {}
/* 0013f2e0 00000008 */ _AptValidate() {}
/* 0013f2e8 000000c4 */ AptLoadAnimation() {}
/* 0013f3b0 00000074 */ AptPreloadAnimation() {}
/* 0013f428 00000064 */ AptCancelPreloadedAnimation() {}
/* 0013f490 00000064 */ AptIsFileLoaded() {}
/* 0013f4f8 0000007c */ static _addScreenGrabToSavedInputs() {}
/* 0013f578 000001d8 */ static _tick() {}
/* 0013f750 00000d30 */ static _playbackSavedInputs() {}
/* 00140480 0000024c */ static _AptInternalUpdate() {}
/* 001406d0 00000064 */ static _AptInternalRender() {}
/* 00140738 00000064 */ AptUpdate() {}
/* 001407a0 0000001c */ AptRender() {}
/* 001407c0 0000005c */ AptSetMousePosition() {}
/* 00140820 00000054 */ AptAddToInputQueue() {}
/* 00140878 00000068 */ AptAddToInputAnalogQueue() {}
/* 001408e0 00000160 */ AptSetInternalVariable() {}
/* 00140a40 00000098 */ AptGetInternalVariable() {}
/* 00140ad8 00000354 */ AptCallFunction() {}
/* 00140e30 00000114 */ AptSetInputRoot() {}
/* 00140f48 00000034 */ AptDebugGetCurrentFrame() {}
/* 00140f80 00000034 */ AptDebugGetNumFrames() {}
/* 00140fb8 00000078 */ AptDebugIsPlaying() {}
/* 00141030 00000014 */ AptDebugEnableSavedInputs() {}
/* 00141048 0000001c */ AptDebugPlaySavedInputs() {}
/* 00141068 0000000c */ AptDebugEnableInputGremlins() {}
/* 00141078 000002b8 */ Apt_atoff() {}
/* 00141330 00000028 */ AptDeallocateAllStrings() {}
/* 00141358 00000014 */ AptSetXMLImplementor() {}
/* 00141370 00000024 */ AptFlushInputQueue() {}
/* 00141398 00000014 */ AptIsMouseOverButton() {}
/* 001413b0 00000098 */ AptRegisterExtension() {}
/* 00141448 0000000c */ AptBreakAfterAssert() {}
/* 00141458 00000010 */ AptPartialGarbageCollection() {}
/* 00141468 0000007c */ static __static_initialization_and_destruction_0() {}
/* 001414e8 00000020 */ _GLOBAL_$I$C__Work_dash_cfe_packages_Apt_Dash_2006_source_Apt_Apt.cppbXaaaa() {}
/* 00141508 00000020 */ _GLOBAL_$D$C__Work_dash_cfe_packages_Apt_Dash_2006_source_Apt_Apt.cppcKbaaa() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_AptLoad.h
// *****************************************************************************

/* ffffffff 0000019c */ AptLinker::~AptLinker() {}
/* ffffffff 000000e8 */ AptSavedInputCheckpoints::~AptSavedInputCheckpoints() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000003e0 */ void EA::String::BasicString<StringAsVectorEncoding<AptFileSavedInputState>, StringAsVectorPolicy>::Insert<AptFileSavedInputState *>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}
/* ffffffff 00000020 */ EAStringC::EAStringC() {}
/* ffffffff 00000028 */ EAStringC::EAStringC() {}
/* ffffffff 0000001c */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}
/* ffffffff 00000074 */ EAStringC::operator=() {}
/* ffffffff 00000048 */ EAStringC::operator==() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptInteger.cpp
// *****************************************************************************

/* 00141528 00000084 */ AptInteger::ClearPool() {}
/* 001415b0 00000014 */ AptInteger::DeleteThis() {}
/* 001415c8 00000014 */ AptInteger::ForceDelete() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/include/Apt/AptValue/AptInteger.h
// *****************************************************************************

/* ffffffff 00000008 */ AptInteger::GetNumGCPointers() {}
/* ffffffff 00000008 */ AptInteger::GetGCPointer() {}
/* ffffffff 00000054 */ AptInteger::~AptInteger() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptInteger.inl
// *****************************************************************************

/* ffffffff 0000009c */ AptInteger::AptInteger() {}
/* ffffffff 00000110 */ AptInteger::Create() {}
/* ffffffff 00000014 */ AptInteger::Destroy() {}
/* ffffffff 00000008 */ AptInteger::GetInt() {}
/* ffffffff 00000024 */ AptInteger::operator new() {}
/* ffffffff 00000024 */ AptInteger::operator delete() {}
/* ffffffff 00000024 */ AptInteger::operator new []() {}
/* ffffffff 00000024 */ AptInteger::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptScriptFunction.cpp
// *****************************************************************************

/* 001415e0 00000160 */ AptScriptFunction::AptScriptFunction() {}
/* 00141740 00000078 */ AptScriptFunction::~AptScriptFunction() {}
/* 001417b8 00000008 */ AptScriptFunction::PreDestroy() {}
/* 001417c0 00000044 */ AptScriptFunction::GetNumGCPointers() {}
/* 00141808 0000009c */ AptScriptFunction::GetGCPointer() {}
/* 001418a8 00000068 */ AptScriptFunction::DestroyGCPointers() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptNativeHash.h
// *****************************************************************************

/* ffffffff 00000010 */ AptNativeHash::ConvertAptValue() {}
/* ffffffff 000000a8 */ AptNativeHash::SetProtoGC() {}
/* ffffffff 000000a8 */ AptNativeHash::SetPrototypeGC() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptScriptFunction.h
// *****************************************************************************

/* ffffffff 00000008 */ AptScriptFunction::getFunction() {}
/* ffffffff 0000003c */ AptScriptFunction::operator new() {}
/* ffffffff 00000044 */ AptScriptFunction::operator delete() {}
/* ffffffff 00000008 */ AptScriptFunction::operator new []() {}
/* ffffffff 00000008 */ AptScriptFunction::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_AptValue.h
// *****************************************************************************

/* ffffffff 0000001c */ AptPrototype::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptPrototype::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptPrototype::ContainsNativeHashVirtual() {}
/* ffffffff 00000008 */ AptPrototype::IsGarbageCollected() {}
/* ffffffff 0000001c */ AptPrototype::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptPrototype::GetGCPointer() {}
/* ffffffff 00000078 */ AptPrototype::~AptPrototype() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptSound.cpp
// *****************************************************************************

/* 00141a50 00000078 */ AptSound::AptSound() {}
/* 00141ac8 00000094 */ AptSound::CleanNativeFunctions() {}
/* 00141b60 00000168 */ AptSound::sMethod_attachSound() {}
/* 00141cc8 0000005c */ AptSound::sMethod_start() {}
/* 00141d28 0000000c */ AptSound::sMethod_stop() {}
/* 00141d38 00000280 */ AptSound::objectMemberLookup() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptSoundMembers.h
// *****************************************************************************

/* 00141910 00000140 */ SoundMembersIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptNativeFunction.h
// *****************************************************************************

/* ffffffff 00000078 */ AptNativeFunction::~AptNativeFunction() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_Apt.h
// *****************************************************************************

/* ffffffff 0000003c */ AptSound::operator new() {}
/* ffffffff 00000044 */ AptSound::operator delete() {}
/* ffffffff 00000008 */ AptSound::operator new []() {}
/* ffffffff 00000008 */ AptSound::operator delete []() {}
/* ffffffff 00000078 */ AptSound::~AptSound() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptBoolean.cpp
// *****************************************************************************

/* 00141fb8 00000084 */ AptBoolean::ClearPool() {}
/* 00142040 00000014 */ AptBoolean::DeleteThis() {}
/* 00142058 00000014 */ AptBoolean::ForceDelete() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/include/Apt/AptValue/AptBoolean.h
// *****************************************************************************

/* ffffffff 00000008 */ AptBoolean::GetNumGCPointers() {}
/* ffffffff 00000008 */ AptBoolean::GetGCPointer() {}
/* ffffffff 00000054 */ AptBoolean::~AptBoolean() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptBoolean.inl
// *****************************************************************************

/* ffffffff 0000009c */ AptBoolean::AptBoolean() {}
/* ffffffff 00000110 */ AptBoolean::Create() {}
/* ffffffff 00000014 */ AptBoolean::Destroy() {}
/* ffffffff 00000008 */ AptBoolean::GetBool() {}
/* ffffffff 00000024 */ AptBoolean::operator new() {}
/* ffffffff 00000024 */ AptBoolean::operator delete() {}
/* ffffffff 00000024 */ AptBoolean::operator new []() {}
/* ffffffff 00000024 */ AptBoolean::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptSpriteMembers.cpp
// *****************************************************************************

/* 00142070 00000180 */ SpriteMembersIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.cpp
// *****************************************************************************

/* 001421f0 000000a0 */ EAStringC::EAStringC() {}
/* 00142290 000000d0 */ EAStringC::EAStringC() {}
/* 00142360 00000128 */ EAStringC::operator+() {}
/* 00142488 000000dc */ EAStringC::operator+=() {}
/* 00142568 00000134 */ EAStringC::operator+() {}
/* 001426a0 0000010c */ EAStringC::operator+=() {}
/* 001427b0 00000134 */ operator+() {}
/* 001428e8 0000008c */ EAStringC::Duplicate() {}
/* 00142978 0000003c */ EAStringC::ReserveSize() {}
/* 001429b8 00000010 */ EAStringC::IsEnoughSize() {}
/* 001429c8 000000b4 */ EAStringC::Append() {}
/* 00142a80 000000f8 */ EAStringC::AppendFormat() {}
/* 00142b78 000000dc */ EAStringC::Format() {}
/* 00142c58 00000068 */ EAStringC::Find() {}
/* 00142cc0 00000070 */ EAStringC::Find() {}
/* 00142d30 00000008 */ EAStringC::FindOneOf() {}
/* 00142d38 00000008 */ EAStringC::ReverseFind() {}
/* 00142d40 00000158 */ EAStringC::Delete() {}
/* 00142e98 00000040 */ EAStringC::Remove() {}
/* 00142ed8 00000008 */ EAStringC::Insert() {}
/* 00142ee0 00000008 */ EAStringC::Insert() {}
/* 00142ee8 0000021c */ EAStringC::Replace() {}
/* 00143108 0000005c */ EAStringC::Replace() {}
/* 00143168 000000d8 */ EAStringC::Left() {}
/* 00143240 000000d4 */ EAStringC::Right() {}
/* 00143318 000000d0 */ EAStringC::Mid() {}
/* 001433e8 000000d8 */ EAStringC::Mid() {}
/* 001434c0 0000004c */ EAStringC::MakeLower() {}
/* 00143510 0000004c */ EAStringC::MakeUpper() {}
/* 00143560 0000009c */ EAStringC::MakeReverse() {}
/* 00143600 00000190 */ EAStringC::TrimLeft() {}
/* 00143790 00000190 */ EAStringC::TrimRight() {}
/* 00143920 0000003c */ EAStringC::Trim() {}
/* 00143960 0000005c */ EAStringC::StartWith() {}
/* 001439c0 00000074 */ EAStringC::EndWith() {}
/* 00143a38 000000ec */ EAStringC::StartWithRemove() {}
/* 00143b28 00000100 */ EAStringC::EndWithRemove() {}
/* 00143c28 0000006c */ EAStringC::UTF8_GetBuffer() {}
/* 00143c98 00000110 */ EAStringC::UTF8_CharAt() {}
/* 00143da8 00000044 */ EAStringC::UTF8_Size() {}
/* 00143df0 000000cc */ EAStringC::UTF8_Mid() {}
/* 00143ec0 00000148 */ EAStringC::UTF8_Mid() {}
/* 00144008 00000104 */ EAStringC::UTF8_Append() {}
/* 00144110 00000124 */ EAStringC::UTF8_Find() {}
/* 00144238 000001e0 */ EAStringC::UTF8_MakeLower() {}
/* 00144418 000001e0 */ EAStringC::UTF8_MakeUpper() {}
/* 001445f8 0000019c */ EAStringC::UTF8_Initialize() {}
/* 00144798 0000018c */ EAStringC::ChangeBuffer() {}
/* 00144928 000000cc */ EAStringC::InitFromBuffer() {}
/* 001449f8 00000068 */ EAStringC::CalculateHashValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 000000bc */ EAStringC::UTF8_ReadCharacter() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptXml.cpp
// *****************************************************************************

/* 00144a60 0000003c */ AptXml::AptXml() {}
/* 00144aa0 000000b0 */ AptXml::AptXml() {}
/* 00144b50 00000068 */ AptXml::~AptXml() {}
/* 00144bb8 00000044 */ AptXml::PreDestroy() {}
/* 00144c00 000000bc */ AptXml::CleanNativeFunctions() {}
/* 00144cc0 00000878 */ AptXml::objectMemberLookup() {}
/* 00145538 00000148 */ AptXml::sMethod_getBytesTotal() {}
/* 00145680 00000148 */ AptXml::sMethod_getBytesLoaded() {}
/* 001457c8 000000ec */ AptXml::sMethod_load() {}
/* 001458b8 000000ec */ AptXml::sMethod_parseXml() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptXml.h
// *****************************************************************************

/* ffffffff 0000003c */ AptXml::operator new() {}
/* ffffffff 00000044 */ AptXml::operator delete() {}
/* ffffffff 00000008 */ AptXml::operator new []() {}
/* ffffffff 00000008 */ AptXml::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptNativeFunction.h
// *****************************************************************************

/* ffffffff 00000078 */ AptNativeFunction::~AptNativeFunction() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptXmlMembers.h
// *****************************************************************************

/* ffffffff 0000016c */ XmlMemberIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}
/* ffffffff 00000028 */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}
/* ffffffff 00000074 */ EAStringC::operator=() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptRenderingContext.cpp
// *****************************************************************************

/* 001459a8 00000050 */ AptRenderingContext::AptRenderingContext() {}
/* 001459f8 00000058 */ AptRenderingContext::pushColourTransform() {}
/* 00145a50 0000005c */ AptRenderingContext::popColourTransform() {}
/* 00145ab0 000000a8 */ AptRenderingContext::appendColourTransform() {}
/* 00145b58 00000080 */ AptRenderingContext::getVertexMatrix() {}
/* 00145bd8 0000004c */ AptRenderingContext::pushVertexMatrix() {}
/* 00145c28 00000070 */ AptRenderingContext::popVertexMatrix() {}
/* 00145c98 00000104 */ AptRenderingContext::multMatrix() {}
/* 00145da0 0000003c */ AptRenderingContext::appendVertexMatrix() {}
/* 00145de0 00000144 */ AptRenderingContext::expandBoundingRect() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptRand.cpp
// *****************************************************************************

/* 00145f28 000001c4 */ static _randReloadMersenneTwister() {}
/* 001460f0 0000007c */ AptRand() {}
/* 00146170 0000004c */ AptSeedRand() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue.cpp
// *****************************************************************************

/* 001463d8 00000098 */ AptValue::toInteger() {}
/* 00146470 000000c8 */ AptValue::toFloat() {}
/* 00146538 0000007c */ AptValue::toString() {}
/* 001465b8 00000354 */ AptValue::toString() {}
/* 00146910 0000000c */ static _constructorObject() {}
/* 00146920 00000030 */ gpObjRegHashGetNumGCPointers() {}
/* 00146950 00000030 */ gpObjRegHashGetGCPointer() {}
/* 00146980 00000324 */ static _gObjRegistrationFunc() {}
/* 00146ca8 00000ac4 */ static _constructBuiltInObjects() {}
/* 00147770 00000608 */ AptValueInitialize() {}
/* 00147d78 00000204 */ AptValueShutdown() {}
/* 00147f80 0000004c */ AptValueShutdownRemaining() {}
/* 00147fd0 00000240 */ AptValue::urlEncode() {}
/* 00148210 000002e4 */ AptValue::urlEncodeCustomRender() {}
/* 001484f8 000005b0 */ AptValue::findChild() {}
/* 00148aa8 0000010c */ AptValue::isMCInParentChain() {}
/* 00148bb8 0000002c */ AptValue::AddRef() {}
/* 00148be8 000000c4 */ AptValue::Release() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObjects.h
// *****************************************************************************

/* 001461c0 00000218 */ ObjectIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/include/Apt/AptValue/AptValue.h
// *****************************************************************************

/* ffffffff 00000064 */ AptValue::ForceDelete() {}
/* ffffffff 00000008 */ AptValue::IsGarbageCollected() {}
/* ffffffff 00000008 */ AptValue::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptValue::ContainsNativeHashVirtual() {}
/* ffffffff 00000008 */ AptValue::objectMemberLookup() {}
/* ffffffff 00000008 */ AptValue::objectMemberSet() {}
/* ffffffff 00000034 */ AptValue::DeleteThis() {}
/* ffffffff 00000008 */ AptValue::PreDestroy() {}
/* ffffffff 00000008 */ AptValue::DestroyGCPointers() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptNativeHash.h
// *****************************************************************************

/* ffffffff 00000010 */ AptNativeHash::ConvertAptValue() {}
/* ffffffff 000000a8 */ AptNativeHash::SetProtoGC() {}
/* ffffffff 000000a8 */ AptNativeHash::SetPrototypeGC() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_AptValue.h
// *****************************************************************************

/* ffffffff 0000001c */ AptPrototype::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptPrototype::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptPrototype::ContainsNativeHashVirtual() {}
/* ffffffff 00000008 */ AptPrototype::IsGarbageCollected() {}
/* ffffffff 0000001c */ AptPrototype::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptPrototype::GetGCPointer() {}
/* ffffffff 00000078 */ AptPrototype::~AptPrototype() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptNativeFunction.h
// *****************************************************************************

/* ffffffff 00000078 */ AptNativeFunction::~AptNativeFunction() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_Apt.h
// *****************************************************************************

/* ffffffff 00000050 */ AptHash::~AptHash() {}
/* ffffffff 00000008 */ AptNone::AddRef() {}
/* ffffffff 00000008 */ AptNone::Release() {}
/* ffffffff 00000008 */ AptNone::GetNumGCPointers() {}
/* ffffffff 00000008 */ AptNone::GetGCPointer() {}
/* ffffffff 0000005c */ AptNone::~AptNone() {}
/* ffffffff 00000008 */ AptExtern::AddRef() {}
/* ffffffff 00000008 */ AptExtern::Release() {}
/* ffffffff 0000007c */ AptExtern::objectMemberSet() {}
/* ffffffff 00000008 */ AptExtern::GetNumGCPointers() {}
/* ffffffff 00000008 */ AptExtern::GetGCPointer() {}
/* ffffffff 0000005c */ AptExtern::~AptExtern() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptGlobalExtensionObject.h
// *****************************************************************************

/* ffffffff 00000008 */ AptGlobalExtensionObject::AddRef() {}
/* ffffffff 00000008 */ AptGlobalExtensionObject::Release() {}
/* ffffffff 0000001c */ AptGlobalExtensionObject::DestroyGCPointers() {}
/* ffffffff 00000008 */ AptGlobalExtensionObject::getNativeHashVirtual() {}
/* ffffffff 00000008 */ AptGlobalExtensionObject::ContainsNativeHashVirtual() {}
/* ffffffff 00000008 */ AptGlobalExtensionObject::IsGarbageCollected() {}
/* ffffffff 0000001c */ AptGlobalExtensionObject::GetNumGCPointers() {}
/* ffffffff 0000001c */ AptGlobalExtensionObject::GetGCPointer() {}
/* ffffffff 00000078 */ AptGlobalExtensionObject::~AptGlobalExtensionObject() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValue.inl
// *****************************************************************************

/* ffffffff 0000007c */ AptValue::AptValue() {}
/* ffffffff 0000006c */ AptValue::AptValue() {}
/* ffffffff 00000030 */ AptValue::~AptValue() {}
/* ffffffff 0000000c */ AptValue::getRefCount() {}
/* ffffffff 0000000c */ AptValue::getVtblIndex() {}
/* ffffffff 00000010 */ AptValue::getGCMark() {}
/* ffffffff 00000010 */ AptValue::getGCRoot() {}
/* ffffffff 0000000c */ AptValue::getIsDefined() {}
/* ffffffff 00000020 */ AptValue::setRefCount() {}
/* ffffffff 0000001c */ AptValue::setVtblIndex() {}
/* ffffffff 00000028 */ AptValue::setIsDefined() {}
/* ffffffff 00000028 */ AptValue::setGCMark() {}
/* ffffffff 00000040 */ AptValue::setGCRoot() {}
/* ffffffff 00000010 */ AptValue::incGCRoot() {}
/* ffffffff 00000010 */ AptValue::decGCRoot() {}
/* ffffffff 00000014 */ AptValue::SetReleaseAtEnd() {}
/* ffffffff 00000018 */ AptValue::ClearReleaseAtEnd() {}
/* ffffffff 00000010 */ AptValue::IsReleaseAtEnd() {}
/* ffffffff 00000008 */ AptValue::c_lookup() {}
/* ffffffff 00000008 */ AptValue::c_integer() {}
/* ffffffff 00000008 */ AptValue::c_register() {}
/* ffffffff 00000008 */ AptValue::c_float() {}
/* ffffffff 00000008 */ AptValue::c_string() {}
/* ffffffff 00000008 */ AptValue::c_boolean() {}
/* ffffffff 00000008 */ AptValue::c_scriptfunction() {}
/* ffffffff 00000008 */ AptValue::c_nativefunction() {}
/* ffffffff 00000008 */ AptValue::c_cih() {}
/* ffffffff 00000008 */ AptValue::c_array() {}
/* ffffffff 00000008 */ AptValue::c_sound() {}
/* ffffffff 00000008 */ AptValue::c_key() {}
/* ffffffff 00000008 */ AptValue::c_mouse() {}
/* ffffffff 00000008 */ AptValue::c_global() {}
/* ffffffff 00000008 */ AptValue::c_math() {}
/* ffffffff 00000008 */ AptValue::c_scriptcolour() {}
/* ffffffff 00000008 */ AptValue::c_object() {}
/* ffffffff 00000008 */ AptValue::c_prototype() {}
/* ffffffff 00000008 */ AptValue::c_date() {}
/* ffffffff 00000008 */ AptValue::c_textformat() {}
/* ffffffff 00000008 */ AptValue::c_movieClip() {}
/* ffffffff 00000008 */ AptValue::c_xmlnode() {}
/* ffffffff 00000008 */ AptValue::c_xml() {}
/* ffffffff 00000008 */ AptValue::c_xmlattributes() {}
/* ffffffff 00000008 */ AptValue::c_loadvars() {}
/* ffffffff 00000014 */ AptValue::isXmlNode() {}
/* ffffffff 00000014 */ AptValue::isXml() {}
/* ffffffff 00000014 */ AptValue::isXmlAttributes() {}
/* ffffffff 00000014 */ AptValue::isLoadVars() {}
/* ffffffff 00000014 */ AptValue::isNone() {}
/* ffffffff 00000010 */ AptValue::isUndefined() {}
/* ffffffff 00000024 */ AptValue::isLookup() {}
/* ffffffff 00000024 */ AptValue::isString() {}
/* ffffffff 00000024 */ AptValue::isBoolean() {}
/* ffffffff 00000024 */ AptValue::isInteger() {}
/* ffffffff 00000024 */ AptValue::isRegister() {}
/* ffffffff 00000024 */ AptValue::isFloat() {}
/* ffffffff 00000024 */ AptValue::isNativeFunction() {}
/* ffffffff 00000024 */ AptValue::isScriptFunction() {}
/* ffffffff 00000024 */ AptValue::isExtern() {}
/* ffffffff 00000024 */ AptValue::isFrameStack() {}
/* ffffffff 00000024 */ AptValue::isArray() {}
/* ffffffff 00000024 */ AptValue::isSound() {}
/* ffffffff 00000024 */ AptValue::isKey() {}
/* ffffffff 00000024 */ AptValue::isMouse() {}
/* ffffffff 00000024 */ AptValue::isMath() {}
/* ffffffff 00000024 */ AptValue::isScriptColour() {}
/* ffffffff 0000003c */ AptValue::isCIH() {}
/* ffffffff 00000024 */ AptValue::isObject() {}
/* ffffffff 00000024 */ AptValue::isPrototype() {}
/* ffffffff 00000024 */ AptValue::isDate() {}
/* ffffffff 00000024 */ AptValue::isTextFormat() {}
/* ffffffff 00000024 */ AptValue::isMovieClip() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}
/* ffffffff 0000000c */ EAStringC::c_str() {}
/* ffffffff 00000020 */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}
/* ffffffff 0000000c */ EAStringC::operator char *() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptDisplayList.cpp
// *****************************************************************************

/* 00148cb0 0000012c */ AptDisplayListState::findInst() {}
/* 00148de0 00000034 */ AptDisplayListState::getLength() {}
/* 00148e18 00000038 */ AptDisplayListState::getValue() {}
/* 00148e50 00000058 */ AptDisplayListState::insert() {}
/* 00148ea8 00000128 */ AptDisplayListState::insert() {}
/* 00148fd0 0000005c */ AptDisplayListState::insert() {}
/* 00149030 0000002c */ AptDisplayListState::remove() {}
/* 00149060 00000038 */ AptDisplayListState::remove() {}
/* 00149098 000006a0 */ AptDisplayList::instantiateCharacter() {}
/* 00149738 0000026c */ _convertToCXForm() {}
/* 001499a8 00000120 */ AptDisplayList::placeObjectNCXForm() {}
/* 00149ac8 0000021c */ AptDisplayList::_addToSetCaches() {}
/* 00149ce8 000002c0 */ AptDisplayList::placeObject() {}
/* 00149fa8 00000234 */ AptDisplayList::placeObject() {}
/* 0014a1e0 000000a4 */ AptDisplayList::removeObject() {}
/* 0014a288 00000040 */ AptDisplayList::removeObject() {}
/* 0014a2c8 0000005c */ AptDisplayList::removeClonedObject() {}
/* 0014a328 0000001c */ AptDisplayList::removeObject() {}
/* 0014a348 0000013c */ AptDisplayList::AptDisplayList() {}
/* 0014a488 00000084 */ AptDisplayList::~AptDisplayList() {}
/* 0014a510 000000c0 */ static _drawCharacterInst() {}
/* 0014a5d0 000001cc */ static _drawCharacterInstOpti() {}
/* 0014a7a0 00000044 */ AptDisplayList::deallocAssetStringRecursive() {}
/* 0014a7e8 000002a0 */ AptDisplayList::render() {}
/* 0014aa88 000000c8 */ AptDisplayList::_getBoundingRect() {}
/* 0014ab50 00000100 */ AptDisplayList::tick() {}
/* 0014ac50 00000094 */ AptDisplayList::clear() {}
/* 0014ace8 00000068 */ AptDisplayList::PreDestroy() {}
/* 0014ad50 00000008 */ AptDisplayList::getState() {}
/* 0014ad58 00000008 */ AptDisplayList::useState() {}
/* 0014ad60 00000748 */ AptDisplayList::mergeState() {}
/* 0014b4a8 000001d4 */ AptDisplayList::validate() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/Display/AptDisplayListState.h
// *****************************************************************************

/* ffffffff 00000024 */ AptDisplayListState::operator delete() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_Apt.h
// *****************************************************************************

/* ffffffff 00000050 */ AptHash::~AptHash() {}
/* ffffffff 00000008 */ AptCharacterInst::PreDestroy() {}
/* ffffffff 0000003c */ AptCharacterInst::DestroyGCPointers() {}
/* ffffffff 00000084 */ AptCharacterShapeInst::~AptCharacterShapeInst() {}
/* ffffffff 00000084 */ AptCharacterStaticTextInst::~AptCharacterStaticTextInst() {}
/* ffffffff 00000084 */ AptCharacterMorphInst::~AptCharacterMorphInst() {}
/* ffffffff 00000050 */ AptCharacterSpriteInst::~AptCharacterSpriteInst() {}
/* ffffffff 0000014c */ AptCharacterTextInst::~AptCharacterTextInst() {}
/* ffffffff 0000007c */ AptCharacterInst::~AptCharacterInst() {}
/* ffffffff 00000024 */ AptCharacterTextInst::operator delete() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000020 */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptInput.cpp
// *****************************************************************************

/* 0014b680 00000120 */ static _getCharacterGridPosition() {}
/* 0014b7a0 0000012c */ static _getButtonScore() {}
/* 0014b8d0 0000027c */ static _getBestButton() {}
/* 0014bb50 0000005c */ _ActionConditionFlagToActionEventFlags() {}
/* 0014bbb0 000001f4 */ AptAnimationPoolData::_doButtonActions() {}
/* 0014bda8 00000158 */ AptAnimationPoolData::setValidFocusButton() {}
/* 0014bf00 00000054 */ static _matrix_vecMult() {}
/* 0014bf58 000000b0 */ static _pointInTri() {}
/* 0014c008 000001a4 */ AptAnimationPoolData::_isPointInButtonHitTestRegion() {}
/* 0014c1b0 00000284 */ AptAnimationPoolData::_getButton() {}
/* 0014c438 000003a0 */ AptAnimationPoolData::_processMouseInput() {}
/* 0014c7d8 00000084 */ AptAnimationPoolData::_pointHits() {}
/* 0014c860 0000038c */ AptAnimationPoolData::_processKeyboardInput() {}
/* 0014cbf0 0000010c */ AptAnimationPoolData::_processAnalogInput() {}
/* 0014cd00 000000fc */ AptAnimationPoolData::_processMouseListenerEvents() {}
/* 0014ce00 000000f4 */ AptAnimationPoolData::_processListenerEvents() {}
/* 0014cef8 00000174 */ AptAnimationPoolData::_addListenerToQueue() {}
/* 0014d070 000004a4 */ AptAnimationPoolData::_handleFocusButton() {}
/* 0014d518 00000284 */ AptAnimationPoolData::_handleAutoNav() {}
/* 0014d7a0 0000010c */ AptAnimationPoolData::_processInput() {}
/* 0014d8b0 00000008 */ AptAnimationPoolData::validateBIL() {}
/* 0014d8b8 00000088 */ AptAnimationPoolData::processInputs() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptMovie.cpp
// *****************************************************************************

/* 0014d940 00000434 */ AptMovie::resolve() {}
/* 0014dd78 000002c8 */ AptMovie::unresolve() {}
/* 0014e040 0000018c */ AptMovie::doFrameControls() {}
/* 0014e1d0 000000c0 */ AptMovie::runFrameActions() {}
/* 0014e290 000000cc */ AptMovie::queueFrameActions() {}
/* 0014e360 00000034 */ AptMovie::labelToFrame() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/_Apt.h
// *****************************************************************************

/* ffffffff 00000050 */ AptHash::~AptHash() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptFastStack.cpp
// *****************************************************************************

/* 0014e398 00000088 */ AptFastStack::Begin() {}
/* 0014e420 00000084 */ AptFastStack::End() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptXmlNode.cpp
// *****************************************************************************

/* 0014e4a8 00000064 */ AptXmlNode::AptXmlNode() {}
/* 0014e510 00000008 */ AptXmlNode::PreDestroy() {}
/* 0014e518 00000080 */ AptXmlNode::~AptXmlNode() {}
/* 0014e598 0000010c */ AptXmlNode::CleanNativeFunctions() {}
/* 0014e6a8 000001b0 */ AptXmlNode::objectMemberSet() {}
/* 0014e858 00000b50 */ AptXmlNode::objectMemberLookup() {}
/* 0014f3a8 0000000c */ AptXmlNode::sMethod_appendChild() {}
/* 0014f3b8 0000000c */ AptXmlNode::sMethod_cloneNode() {}
/* 0014f3c8 00000258 */ AptXmlNode::sMethod_hasChildNodes() {}
/* 0014f620 0000000c */ AptXmlNode::sMethod_insertBefore() {}
/* 0014f630 0000000c */ AptXmlNode::sMethod_removeNode() {}
/* 0014f640 00000160 */ AptXmlNode::sMethod_toString() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptXmlNode.h
// *****************************************************************************

/* ffffffff 0000003c */ AptXmlNode::operator new() {}
/* ffffffff 00000044 */ AptXmlNode::operator delete() {}
/* ffffffff 00000008 */ AptXmlNode::operator new []() {}
/* ffffffff 00000008 */ AptXmlNode::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptNativeFunction.h
// *****************************************************************************

/* ffffffff 00000078 */ AptNativeFunction::~AptNativeFunction() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptXmlMembers.h
// *****************************************************************************

/* ffffffff 0000016c */ XmlMemberIndex::in_word_set() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptValue/AptValueVector.inl
// *****************************************************************************

/* ffffffff 00000048 */ AptValueVector::PushValue() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/string/EAString.inl
// *****************************************************************************

/* ffffffff 00000018 */ EAStringC::IsEmpty() {}
/* ffffffff 00000028 */ EAStringC::EAStringC() {}
/* ffffffff 0000006c */ EAStringC::~EAStringC() {}
/* ffffffff 00000074 */ EAStringC::operator=() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptMath.cpp
// *****************************************************************************

/* 0014f7a0 000000dc */ AptMath::ClipStackMakeUnit() {}
/* 0014f880 00000028 */ AptMath::ClipStackPushUnit() {}
/* 0014f8a8 00000044 */ AptMath::ClipStackInit() {}
/* 0014f8f0 0000000c */ AptMath::ClipStackShutdown() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptXmlAttributes.cpp
// *****************************************************************************

/* 0014f900 00000060 */ AptXmlAttributes::AptXmlAttributes() {}
/* 0014f960 0000007c */ AptXmlAttributes::~AptXmlAttributes() {}
/* 0014f9e0 00000008 */ AptXmlAttributes::PreDestroy() {}
/* 0014f9e8 00000078 */ AptXmlAttributes::objectMemberSet() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/Apt/AptObject/AptXmlAttributes.h
// *****************************************************************************

/* ffffffff 0000003c */ AptXmlAttributes::operator new() {}
/* ffffffff 00000044 */ AptXmlAttributes::operator delete() {}
/* ffffffff 00000008 */ AptXmlAttributes::operator new []() {}
/* ffffffff 00000008 */ AptXmlAttributes::operator delete []() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/AptAuxEAGLREAL/AptAuxEAGLREAL.cpp
// *****************************************************************************

/* 0014fb58 00000028 */ MemAlloc() {}
/* 0014fb80 0000001c */ MemFree() {}
/* 0014fba0 00000024 */ _assertFail() {}
/* 0014fbc8 00000008 */ AssertFail() {}
/* 0014fbd0 000000f4 */ static DebugPrint() {}
/* 0014fcc8 00000008 */ static DebugSetScreenGrabPending() {}
/* 0014fcd0 00000008 */ static DebugAddSavedInput() {}
/* 0014fcd8 000000ac */ static _findModel() {}
/* 0014fd88 00000098 */ static _getModelPointers() {}
/* 0014fe20 000000ec */ static _loadGeometry() {}
/* 0014ff10 00000094 */ static _loadSoundBank() {}
/* 0014ffa8 000000c4 */ static loadTexture() {}
/* 00150070 00000008 */ static freeTexture() {}
/* 00150078 0000026c */ static bindTexture() {}
/* 001502e8 00000034 */ static command() {}
/* 00150320 0000004c */ static _loadFileSize() {}
/* 00150370 00000034 */ static isUTF8File() {}
/* 001503a8 00000010 */ static isUCS2File() {}
/* 001503b8 00000040 */ static UTF8Size() {}
/* 001503f8 000000cc */ static UCS2ToUTF8() {}
/* 001504c8 00000124 */ getBytesTotal() {}
/* 001505f0 00000034 */ getBytesLoaded() {}
/* 00150628 00000040 */ sendVariables() {}
/* 00150668 00000048 */ static loadVariablesNULL() {}
/* 001506b0 00000508 */ static GetExternVariable() {}
/* 00150bb8 000001c0 */ loadVariables() {}
/* 00150d78 0000009c */ static loadSound() {}
/* 00150e18 00000008 */ static freeSound() {}
/* 00150e20 00000040 */ static startSound() {}
/* 00150e60 000000a0 */ static startSoundStream() {}
/* 00150f00 00000050 */ static SetBackgroundColour() {}
/* 00150f50 00000158 */ static SetVertexMatrix() {}
/* 001510a8 00000080 */ AptAuxEAGLREAL_TransformPoint() {}
/* 00151128 00000058 */ static SetColourTransform() {}
/* 00151180 000001dc */ static _transformColour() {}
/* 00151360 0000001c */ static loadRenderingUnit() {}
/* 00151380 00000008 */ static freeRenderingUnit() {}
/* 00151388 00000084 */ static customControlRender() {}
/* 00151410 0000035c */ static _clearZToDepth() {}
/* 00151770 00000120 */ static _setDepthMethod() {}
/* 00151890 0000000c */ AptAuxEAGLREAL_NeverClearZ() {}
/* 001518a0 000000a4 */ static drawRenderingUnit() {}
/* 00151948 00000340 */ CommonDraw() {}
/* 00151c88 000001f4 */ static _createShape() {}
/* 00151e80 00000108 */ AptAuxEAGLREAL_GetRealTimeClock() {}
/* 00151f88 00000140 */ AptAuxEAGLREAL_CreateFontTexture() {}
/* 001520c8 00000174 */ From8BitsTo4Bits() {}
/* 00152240 00000720 */ static allocateStringFF() {}
/* 00152960 00000008 */ AptAuxEAGLREAL_GetSHAPEFromString() {}
/* 00152968 00000220 */ AptAuxEAGLREAL_StartBatchDraw() {}
/* 00152b88 00000064 */ AptAuxEAGLREAL_EndBatchDraw() {}
/* 00152bf0 00000034 */ AptAuxEAGLREAL_AddStringVertex() {}
/* 00152c28 000006b0 */ static _drawStringRaw() {}
/* 001532d8 0000006c */ AptAuxEAGLREAL_DrawStringIgnoreZ() {}
/* 00153348 00000094 */ static drawStringFF() {}
/* 001533e0 000000c8 */ static deallocateStringFF() {}
/* 001534a8 00000028 */ AddDelayedReleasePointer() {}
/* 001534d0 00000098 */ FreeDelayedReleasePointers() {}
/* 00153568 0000001c */ AptAuxEAGLREAL_TimerInit() {}
/* 00153588 00000024 */ AptAuxEAGLREAL_TimerStartFrame() {}
/* 001535b0 00000028 */ AptAuxEAGLREAL_TimerGetTicks() {}
/* 001535d8 0000001c */ AptAuxEAGLREAL_TimerGetTime() {}
/* 001535f8 00000008 */ LoadAnimationCompleted() {}
/* 00153600 0000000c */ AptAuxEAGLREAL_SetAsyncLoadIntoMemFlags() {}
/* 00153610 0000000c */ AptAuxEAGLREAL_GetAsyncLoadIntoMemFlags() {}
/* 00153620 0000000c */ AptAuxEAGLREAL_SetAsyncLoadUseTag() {}
/* 00153630 0000000c */ AptAuxEAGLREAL_GetAsyncLoadUseTag() {}
/* 00153640 0000000c */ AptAuxEAGLREAL_SetAsyncDecompressIntoMemFlags() {}
/* 00153650 0000000c */ AptAuxEAGLREAL_GetAsyncDecompressIntoMemFlags() {}
/* 00153660 000002c4 */ static LoadAnimation() {}
/* 00153928 00000154 */ AptAuxEAGLREAL_ResolveData() {}
/* 00153a80 00000304 */ AptAuxEAGLREAL_FinishAsyncLoads() {}
/* 00153d88 00000008 */ static FreeConstantTable() {}
/* 00153d90 0000008c */ AptAuxEAGLREAL_ReleaseLayerInfo() {}
/* 00153e20 00000048 */ static FreeAnimation() {}
/* 00153e68 00000220 */ AptAuxEAGLREAL_SetupCallbacks() {}
/* 00154088 0000000c */ AptAuxEAGLREAL_SetFontTextureMemoryPoolSize() {}
/* 00154098 00000080 */ _allocFontTextureMemoryPool() {}
/* 00154118 00000364 */ AptAuxEAGLREAL_InitializeFonts() {}
/* 00154480 000002f4 */ AptAuxEAGLREAL_ShutdownFonts() {}
/* 00154778 000001c4 */ AptAuxEAGLREAL_Initialize() {}
/* 00154940 0000001c */ AptAuxEAGLREAL_LoadNetworkLibraries() {}
/* 00154960 000000b8 */ AptAuxEAGLREAL_CleanUpOldStrings() {}
/* 00154a18 0000000c */ AptAuxEAGLREAL_GetCustomControlMatrix() {}
/* 00154a28 0000008c */ static __static_initialization_and_destruction_0() {}
/* 00154ab8 00000020 */ _GLOBAL_$I$C__Work_dash_cfe_packages_Apt_Dash_2006_source_AptAuxEAGLREAL_AptAuxEAGLREAL.cpp3Vaaaa() {}
/* 00154ad8 00000020 */ _GLOBAL_$D$C__Work_dash_cfe_packages_Apt_Dash_2006_source_AptAuxEAGLREAL_AptAuxEAGLREAL.cppWHbaaa() {}

// *****************************************************************************
// FILE -- C:/work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}

// *****************************************************************************
// FILE -- C:/work/dash/cfe/packages/easoApt/DASH_2006/source/easoStubCallbacks.h
// *****************************************************************************

/* 0014fa88 00000024 */ NetAlloc() {}
/* 0014fab0 00000024 */ NetFree() {}
/* 0014fad8 0000001c */ LobbyAlloc() {}
/* 0014faf8 0000001c */ LobbyFree() {}
/* 0014fb18 0000001c */ RpcAlloc() {}
/* 0014fb38 0000001c */ RpcFree() {}

// *****************************************************************************
// FILE -- C:/work/dash/cfe/packages/EAGLPS2/4.08.07/include/ps2/eagl/runtime/tar.inl
// *****************************************************************************

/* ffffffff 00000010 */ EAGL::TARExtension::GetWidth() {}
/* 0014fa60 00000014 */ EAGL::TARExtension::GetHeight() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/AptAuxEAGLREAL/AptViewerEasoGI.cpp
// *****************************************************************************

/* 00154b20 0000002c */ EASO::MyLocaleGetstrCallback() {}
/* 00154b50 00000008 */ EASO::GetSharedLoginIconX() {}
/* 00154b58 00000014 */ static __tcf_1() {}
/* 00154b70 00000024 */ static __tcf_2() {}
/* 00154b98 0000004c */ static __tcf_3() {}
/* 00154be8 000000d0 */ EASO::_PS2LoadFile() {}
/* 00154cb8 00000020 */ static EASO::PumpUpdate() {}
/* 00154cd8 00000044 */ EASO::AptViewerEasoGI::AptViewerEasoGI() {}
/* 00154d20 00000030 */ EASO::AptViewerEasoGI::~AptViewerEasoGI() {}
/* 00154d50 00000080 */ EASO::AptViewerEasoGI::EasoAlloc() {}
/* 00154dd0 00000044 */ EASO::AptViewerEasoGI::EasoFree() {}
/* 00154e18 0000013c */ EASO::AptViewerEasoGI::LoadDictFileSize() {}
/* 00154f58 00000024 */ EASO::AptViewerEasoGI::FreeFileMem() {}
/* 00154f80 00000034 */ EASO::AptViewerEasoGI::GetProductId() {}
/* 00154fb8 0000000c */ EASO::AptViewerEasoGI::GetGameName() {}
/* 00154fc8 00000020 */ EASO::AptViewerEasoGI::GetVersion() {}
/* 00154fe8 0000001c */ EASO::AptViewerEasoGI::GetLanguage() {}
/* 00155008 0000001c */ EASO::AptViewerEasoGI::GetDnasRegionNum() {}
/* 00155028 00000034 */ EASO::AptViewerEasoGI::GetSku() {}
/* 00155060 000000ec */ EASO::AptViewerEasoGI::GetRegion() {}
/* 00155150 00000014 */ EASO::AptViewerEasoGI::GetRosterVersion() {}
/* 00155168 00000040 */ EASO::AptViewerEasoGI::GetRosterBuffer() {}
/* 001551a8 00000044 */ EASO::AptViewerEasoGI::ClearRosterBuffer() {}
/* 001551f0 00000028 */ EASO::AptViewerEasoGI::GetLoginData() {}
/* 00155218 00000008 */ EASO::AptViewerEasoGI::GetScoreReport() {}
/* 00155220 0000001c */ EASO::AptViewerEasoGI::EnableDebug() {}
/* 00155240 00000008 */ EASO::AptViewerEasoGI::PauseSong() {}
/* 00155248 00000008 */ EASO::AptViewerEasoGI::ResumeSong() {}
/* 00155250 00000024 */ EASO::AptViewerEasoGI::ShutdownResources() {}
/* 00155278 00000040 */ EASO::AptViewerEasoGI::GetRelaunchGameParam() {}
/* 001552b8 00000008 */ EASO::AptViewerEasoGI::GetDNASRegion() {}
/* 001552c0 00000008 */ EASO::AptViewerEasoGI::GetDNASLanguage() {}
/* 001552c8 00000074 */ EASO::AptViewerEasoGI::GetRelaunchGameElfPath() {}
/* 00155340 00000080 */ EASO::AptViewerEasoGI::AsyncFileCB() {}
/* 001553c0 00000088 */ EASO::AptViewerEasoGI::UnpackEncodedDNASFile() {}
/* 00155448 000000b0 */ EASO::AptViewerEasoGI::LoadDnasElf() {}
/* 001554f8 00000030 */ EASO::AptViewerEasoGI::FreeDnasData() {}
/* 00155528 00000068 */ EASO::AptViewerEasoGI::LoadDnasAuthData() {}
/* 00155590 00000008 */ EASO::AptViewerEasoGI::EnableModem() {}
/* 00155598 00000008 */ EASO::AptViewerEasoGI::GetSharedLoginTerritoryID() {}
/* 001555a0 00000078 */ EASO::AptViewerEasoGI::GetSharedLoginIcon() {}
/* 00155618 0000001c */ EASO::AptViewerEasoGI::GetSharedLoginTimerFrequency() {}
/* 00155638 00000050 */ EASO::AptViewerEasoGI::GetRealMemcardInterface() {}
/* 00155688 0000001c */ EASO::AptViewerEasoGI::GetSharedLoginThreadInterface() {}
/* 001556a8 0000001c */ EASO::AptViewerEasoGI::GetSharedLoginTimerElapsedTicks() {}
/* 001556c8 00000008 */ EASO::AptViewerEasoGI::ShutdownSharedLogin() {}
/* ffffffff 00000050 */ EASO::MySharedLoginAllocator::Instance() {}
/* ffffffff 000000a4 */ EASO::MyMutex::Instance() {}
/* ffffffff 00000054 */ EASO::MyMutex::~MyMutex() {}
/* ffffffff 00000064 */ EASO::SharedLoginThreadInterface::~SharedLoginThreadInterface() {}
/* ffffffff 00000098 */ EASO::SharedLoginThreadInterface::Instance() {}
/* ffffffff 00000008 */ EASO::SharedLoginThreadInterface::CreateInstance() {}
/* ffffffff 00000048 */ EASO::SharedLoginThreadInterface::Release() {}
/* ffffffff 00000008 */ EASO::SharedLoginThreadInterface::SetStackSize() {}
/* ffffffff 0000006c */ EASO::SharedLoginThreadInterface::Begin() {}
/* ffffffff 00000038 */ EASO::SharedLoginThreadInterface::WaitForEnd() {}
/* ffffffff 0000001c */ EASO::SharedLoginThreadInterface::Sleep() {}
/* ffffffff 00000028 */ EASO::SharedLoginThreadInterface::SetPriority() {}
/* ffffffff 00000030 */ EASO::MySharedLoginAllocator::~MySharedLoginAllocator() {}
/* ffffffff 0000002c */ EASO::MySharedLoginAllocator::Alloc() {}
/* ffffffff 0000001c */ EASO::MySharedLoginAllocator::Free() {}
/* ffffffff 00000008 */ EASO::MySharedLoginAllocator::AddRef() {}
/* ffffffff 00000008 */ EASO::MySharedLoginAllocator::Release() {}
/* ffffffff 00000074 */ EASO::MyMutex::CreateInstance() {}
/* ffffffff 00000010 */ EASO::MyMutex::AddRef() {}
/* ffffffff 00000048 */ EASO::MyMutex::Release() {}
/* ffffffff 0000001c */ EASO::MyMutex::Lock() {}
/* ffffffff 0000001c */ EASO::MyMutex::Unlock() {}
/* ffffffff 00000050 */ EASO::SharedLoginThreadInterface::EntryProc() {}
/* ffffffff 00000008 */ EASO::SharedLoginThreadInterface::GetEntryFunc() {}
/* ffffffff 00000008 */ EASO::SharedLoginThreadInterface::IsActive() {}

// *****************************************************************************
// FILE -- C:/work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}

// *****************************************************************************
// FILE -- C:/work/dash/cfe/packages/Allocator/1.4.0/include/allocator/iallocator.h
// *****************************************************************************

/* ffffffff 00000030 */ EA::Allocator::IAllocator::~IAllocator() {}

// *****************************************************************************
// FILE -- C:/work/dash/cfe/packages/realmemcard/2.02.03/include/common/realmemcard/memcard.h
// *****************************************************************************

/* 00154af8 00000008 */ Realmc::IThread::AddRef() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/AptAuxEAGLREAL/timer.cpp
// *****************************************************************************

/* 001556d0 00000050 */ Timer_Init() {}
/* 00155720 00000140 */ Timer_StartFrame() {}
/* 00155860 0000000c */ Timer_GetTime() {}
/* 00155870 0000000c */ Timer_GetTicks() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/Apt/Dash_2006/source/AptFF/ffapt.cpp
// *****************************************************************************

/* 00155880 00000024 */ CFontFusion::CFontFusion() {}
/* 001558a8 00000024 */ CFontFusion::flushCache() {}
/* 001558d0 00000188 */ CFontFusion::init() {}
/* 00155a58 000000ac */ CFontFusion::purge() {}
/* 00155b08 00000024 */ CFontFusion::~CFontFusion() {}
/* 00155b30 00000010 */ CFontFusion::setStrokeFontScale() {}
/* 00155b40 000000d8 */ CFontFusion::getName() {}
/* 00155c18 000001bc */ CFontFusion::HAK_MeasureTextInX() {}
/* 00155dd8 000001f0 */ CFontFusion::AnalyzeLine() {}
/* 00155fc8 00000704 */ CFontFusion::renderString() {}
/* 001566d0 00000278 */ CFontFusion::calcRenderStringSize() {}
/* 00156948 00000040 */ static __static_initialization_and_destruction_0() {}
/* 00156988 00000020 */ _GLOBAL_$I$C__Work_dash_cfe_packages_Apt_Dash_2006_source_AptFF_ffapt.cppjfbaaa() {}
/* 001569a8 00000020 */ _GLOBAL_$D$C__Work_dash_cfe_packages_Apt_Dash_2006_source_AptFF_ffapt.cppskcaaa() {}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\fontmanager\ff_fm.c
// *****************************************************************************

/* 001569c8 0000044c */ void FF_FM_RenderGlyph(/* s0 16 */ FF_FM_Class *pFM, /* a1 5 */ uint16 fontCode, /* s2 18 */ T2K **pScaler, /* s4 20 */ int charCode, /* 0x4(sp) */ int8 xFracPenDelta, /* 0x8(sp) */ int8 yFracPenDelta, /* 0xc(sp) */ uint8 greyScaleLevel, /* s5 21 */ uint16 cmd, /* 0xc0(sp) */ int *pErrCode) {
	/* v0 2 */ int *pErrCode;
	/* 0x0(sp) */ int errCode;
	/* t5 13 */ uint32 hashIndex;
	/* s1 17 */ char done;
	/* s1 17 */ char defaultChar;
	/* s3 19 */ uint16 scalerIndex;
	/* v0 2 */ uint16 gIndex;
	/* v0 2 */ int numGlyphs;
	/* fp 30 */ int totGlyphs;
	/* a2 6 */ int oldTotGlyphs;
}

/* 00156e18 00000418 */ T2K* FF_FM_SelectFont(/* s4 20 */ FF_FM_Class *pFM, /* 0x14(sp) */ uint16 fontCode, /* 0x18(sp) */ int *errCode) {
	/* 0x1c(sp) */ uint32 hashIndex;
	/* 0x10(sp) */ int err;
	/* 0x20(sp) */ uint16 logFontIndex;
	/* s1 17 */ InputStream *in1;
	/* s3 19 */ InputStream *in2;
	/* 0x24(sp) */ int nScalersNeeded;
	/* 0x28(sp) */ uint16 firstCharCode;
	/* 0x2c(sp) */ uint16 lastCharCode;
	/* 0x0(sp) */ T2K_TRANS_MATRIX trans;
	/* 0x30(sp) */ T2K_AlgStyleDescriptor *stylePtr;
	/* s6 22 */ int scalerIndex;
}

/* 00157230 00000090 */ void FF_FM_DeleteFont(/* s0 16 */ FF_FM_Class *pFM, /* a1 5 */ uint16 fontCode, /* s1 17 */ int *errCode) {
	/* v1 3 */ uint32 hashIndex;
}

/* 001572c0 00000124 */ uint16 FF_FM_CreateFont(/* a0 4 */ FF_FM_Class *pFM, /* a1 5 */ uint16 index, /* a2 6 */ char *flushCache, /* a3 7 */ T2K_TRANS_MATRIX *trans, /* t0 8 */ T2K_AlgStyleDescriptor *styling, /* t1 9 */ int *errCode) {
	/* t2 10 */ uint32 hashIndex;
	/* t8 24 */ int err;
	/* t3 11 */ uint16 newFontCode;
	/* t4 12 */ char done;
}

/* 001573e8 000000e4 */ void FF_FM_AddTypefaceStream(/* s0 16 */ FF_FM_Class *p, /* s1 17 */ InputStream *InputStreamA, /* s2 18 */ InputStream *InputStreamB, /* s3 19 */ int *pErrCode) {
	/* v0 2 */ int errCode;
	/* a1 5 */ size_t size;
}

/* 001574d0 000000f4 */ FF_FM_Class* FF_FM_New(/* s1 17 */ int *pErrCode) {
	/* s0 16 */ FF_FM_Class *p;
}

/* 001575c8 0000005c */ void FF_FM_Delete(/* s0 16 */ FF_FM_Class *pFM, /* s1 17 */ int *errCode) {}

/* 00157628 000000a8 */ static void ff_fm_EmergencyShutDown(/* s1 17 */ FF_FM_Class *pFM) {
	/* s0 16 */ uint32 hashIndex;
}

/* 001576d0 000000b4 */ void FF_FM_EnumTypefaces(/* s1 17 */ FF_FM_Class *pFM, /* s2 18 */ int (*enumTypefaceCallBack)(/* parameters unknown */)) {
	/* s0 16 */ uint16 logFontIdx;
	/* a0 4 */ char more;
	/* a1 5 */ uint8 *faceName8;
	/* a2 6 */ uint16 *faceName16;
}

/* 00157788 00000030 */ static void RemoveStreamsFM(/* s0 16 */ FF_FM_Class *p) {}
/* 001577b8 00000030 */ static void RemovePhysFontsFM(/* s0 16 */ FF_FM_Class *p) {}

/* 001577e8 00000074 */ static void RemoveLogFontsFM(/* s1 17 */ FF_FM_Class *p) {
	/* s0 16 */ int ii;
}

/* 00157860 0000001c */ static int AddToPhysicalAndLogicalFontLists(/* a0 4 */ FF_FM_Class *pFM, /* a1 5 */ unsigned int streamIndex) {}

/* 00157880 000001d0 */ static int AddPhysicalFont(/* s0 16 */ FF_FM_Class *pFM, /* s4 20 */ T2K *scaler, /* s1 17 */ unsigned int streamIndex, /* s2 18 */ unsigned int subIndex) {
	/* 0x0(sp) */ int errCode;
	/* a1 5 */ size_t size;
	/* a1 5 */ char *src;
	/* s5 21 */ char *dst;
	/* s2 18 */ uint16 *src16;
	/* s3 19 */ uint16 *dst16;
	/* a1 5 */ int ii;
}

/* 00157a50 0000008c */ static int str16cmp(/* t1 9 */ uint16 *s1_16, /* a1 5 */ uint16 *s2_16) {}

/* 00157ae0 00000404 */ static int AddToLogicalFontList(/* s1 17 */ FF_FM_Class *pFM, /* 0x80(sp) */ unsigned int physicalFontIndex) {
	/* 0x84(sp) */ int errCode;
	/* fp 30 */ unsigned int foundIndex;
	/* t6 14 */ char found;
	/* s5 21 */ unsigned int ii;
	/* s2 18 */ unsigned int jj;
	/* v0 2 */ unsigned int fragIndex;
	/* s4 20 */ unsigned int nFragments;
	/* t3 11 */ char *s1;
	/* a1 5 */ char *s2;
	/* a0 4 */ uint16 *s1_16;
	/* t4 12 */ uint16 *s2_16;
	/* 0x0(sp) */ uint16 TmpFontName16[64];
	/* a2 6 */ int kk;
}

/* 00157ee8 000001b0 */ static int AddToPhysicalFontList(/* s2 18 */ FF_FM_Class *pFM, /* s7 23 */ unsigned int streamIndex) {
	/* 0x0(sp) */ int errCode;
	/* s4 20 */ unsigned int ii;
	/* s6 22 */ unsigned int nSubFonts;
	/* s5 21 */ short int fontType;
	/* s3 19 */ InputStream *in;
	/* fp 30 */ InputStream *in2;
	/* s1 17 */ sfntClass *font;
	/* s0 16 */ T2K *localScaler;
	/* s0 16 */ tsiMemObject *mem;
	/* v0 2 */ char done;
}

/* 00158098 00000134 */ static void DoKillScalers(/* s2 18 */ FF_FM_Class *pFM, /* s3 19 */ int *errCode) {
	/* s6 22 */ uint32 hashIndex;
	/* s0 16 */ int scalerIndex;
}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\cachemanager\cachemgr.c
// *****************************************************************************

/* 001581d0 00000068 */ static void CmInitializeCache(/* a0 4 */ void **pCacheContext, /* a1 5 */ int len, /* a2 6 */ char *cacheptr) {
	/* a3 7 */ FF_CM_Class *pCmGlobals;
	/* v1 3 */ int i;
	/* v0 2 */ chardata_hdr **hashtemp;
}

/* 00158238 0000009c */ FF_CM_Class* FF_CM_New(/* s1 17 */ int sizeofCache, /* s0 16 */ int *errCode) {
	/* 0x0(sp) */ FF_CM_Class *pCmGlobals;
	/* s2 18 */ tsiMemObject *mem;
	/* v0 2 */ char *cacheptr;
}

/* 001582d8 00000038 */ void FF_CM_Delete(/* a0 4 */ FF_CM_Class *theCache, /* a1 5 */ int *errCode) {
	/* s0 16 */ tsiMemObject *tempmem;
}

/* 00158310 00000010 */ void FF_CM_SetFilter(/* a0 4 */ FF_CM_Class *theCache, /* a1 5 */ uint16 theFilterTag, /* a2 6 */ FF_T2K_FilterFuncPtr BitmapFilter, /* a3 7 */ void *filterParamsPtr) {}

/* 00158320 00000254 */ void FF_CM_RenderGlyph(/* s4 20 */ FF_CM_Class *theCache, /* s6 22 */ uint16 font_code, /* s5 21 */ T2K **theScaler, /* s3 19 */ int char_code, /* 0x20(sp) */ int8 xFracPenDelta, /* 0x24(sp) */ int8 yFracPenDelta, /* 0x28(sp) */ uint8 greyScaleLevel, /* s2 18 */ uint16 cmd, /* 0xd0(sp) */ int *errCode) {
	/* s7 23 */ int *errCode;
	/* s0 16 */ T2K *tScaler;
	/* s1 17 */ chardata_hdr *tempReturnChar;
	/* 0x10(sp) */ char_desc_t request;
	/* fp 30 */ uint32 inst_code;
	/* t1 9 */ uint16 coreCmd;
}

/* 00158578 000001bc */ int FF_CM_GlyphInCache(/* a0 4 */ FF_CM_Class *theCache, /* a1 5 */ uint16 font_code, /* a2 6 */ T2K **theScaler, /* s3 19 */ int char_code, /* t0 8 */ int8 xFracPenDelta, /* t1 9 */ int8 yFracPenDelta, /* s5 21 */ uint8 greyScaleLevel, /* t3 11 */ uint16 cmd, /* 0x90(sp) */ int *errCode) {
	/* s4 20 */ int *errCode;
	/* s1 17 */ T2K *tScaler;
	/* s0 16 */ chardata_hdr *tempReturnChar;
	/* 0x0(sp) */ char_desc_t request;
	/* t0 8 */ uint32 inst_code;
	/* s6 22 */ int found;
	/* t1 9 */ uint16 coreCmd;
}

/* 00158738 0000003c */ static void* ff_cm_GetMemory(/* s0 16 */ void *cache, /* a1 5 */ int length) {
	/* v0 2 */ chardata_hdr *tempReturnChar;
}

/* 00158778 00000060 */ void FF_CM_Flush(/* a0 4 */ FF_CM_Class *cache, /* a1 5 */ int *errCode) {
	/* v1 3 */ int i;
	/* v0 2 */ chardata_hdr **hashtemp;
}

/* 001587d8 000000cc */ static chardata_hdr* CmFindChar(/* t0 8 */ FF_CM_Class *pCmGlobals, /* a3 7 */ char_desc_t *prequest) {
	/* a1 5 */ chardata_hdr *current;
	/* a0 4 */ chardata_hdr *temp;
	/* a2 6 */ char_desc_t *curreq;
	/* a0 4 */ int match;
}

/* 001588a8 000002ac */ static chardata_hdr* CmMakeChar(/* s2 18 */ T2K **theScaler, /* s3 19 */ int char_code, /* a2 6 */ int8 xFracPenDelta, /* a3 7 */ int8 yFracPenDelta, /* v1 3 */ uint8 greyScaleLevel, /* t3 11 */ uint16 cmd, /* s5 21 */ uint16 font_code, /* s6 22 */ uint32 inst_code, /* 0x90(sp) */ int *errCode) {
	/* s4 20 */ int *errCode;
	/* a0 4 */ chardata_hdr *temp;
	/* a1 5 */ char_desc_t *curreq;
	/* s1 17 */ T2K *tScaler;
	/* s0 16 */ FF_CM_Class *tempCache;
	/* a0 4 */ void *theFM;
}

/* 00158b58 00000158 */ static chardata_hdr* CmAllocMem(/* s0 16 */ FF_CM_Class *pCmGlobals, /* s1 17 */ int len) {
	/* a0 4 */ memory_hdr *curseg;
	/* a2 6 */ chardata_hdr *temp;
}

/* 00158cb0 000000f0 */ static memory_hdr* CmFreeMem(/* a0 4 */ FF_CM_Class *pCmGlobals, /* a1 5 */ chardata_hdr *p) {
	/* v0 2 */ memory_hdr *curseg;
	/* a1 5 */ memory_hdr *freemem;
	/* a3 7 */ memory_hdr *prev;
	/* a2 6 */ memory_hdr *next;
}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\sniffer.c
// *****************************************************************************

/* 00158da0 0000002c */ short int ff_FontTypeFromStream(/* a0 4 */ InputStream *in) {}
/* 00158dd0 00000020 */ short int FF_FontTypeFromStream(/* a0 4 */ InputStream *in, /* a1 5 */ int *errCode) {}

/* 00158df0 000001e8 */ static char IsTT(/* s3 19 */ InputStream *in) {
	/* s0 16 */ char isMe;
	/* 0x0(sp) */ uint8 aBuffer[10];
	/* s1 17 */ sfntClass *tmp_sfnt;
	/* s2 18 */ tsiMemObject *tmp_mem;
	/* 0x10(sp) */ int errCode;
}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\t2k.c
// *****************************************************************************

/* 00158fd8 00000094 */ static dropoutAdaptationClass* New_dropoutAdaptationClass(/* s2 18 */ tsiMemObject *mem, /* s0 16 */ int numGlyphs) {
	/* s1 17 */ dropoutAdaptationClass *t;
}

/* 00159070 00000054 */ static void setDropoutSize(/* a0 4 */ dropoutAdaptationClass *t, /* a1 5 */ int xPPEm, /* a2 6 */ int yPPEm) {
	/* a0 4 */ uint8 *p;
	/* a3 7 */ int i;
	/* t0 8 */ int length;
}

/* 001590c8 0000003c */ static void Delete_dropoutAdaptationClass(/* s0 16 */ dropoutAdaptationClass *t) {}

/* 00159108 00000134 */ T2K* NewT2K(/* s0 16 */ tsiMemObject *mem, /* s1 17 */ sfntClass *font, /* a2 6 */ int *errCode) {
	/* a2 6 */ T2K *t;
	/* t0 8 */ int i;
}

/* 00159240 00000264 */ static void T2KDoGriddingSetUp(/* s0 16 */ T2K *t) {
	/* v0 2 */ int err;
	/* s1 17 */ InputStream *in;
	/* 0x0(sp) */ ag_GlobalDataType globalHints;
}

/* 001594a8 00000154 */ static void scalePoints(/* a0 4 */ int16 *ooz, /* a1 5 */ F26Dot6 *z, /* a2 6 */ int n, /* a3 7 */ T2KScaleInfo *si) {
	/* s0 16 */ int i;
	/* v0 2 */ F26Dot6 tmp32;
	/* t1 9 */ int16 nScale;
	/* t0 8 */ int32 dScaleDiv2;
	/* a3 7 */ int16 dShift;
	/* a3 7 */ int32 dScale;
	/* s3 19 */ F16Dot16 fixedScale;
}

/* 00159600 00000070 */ static void T2K_NewTransformationInternal(/* a0 4 */ T2K *t, /* a1 5 */ int doSetUpNow, /* a2 6 */ int xPixelsPerEm, /* a3 7 */ int yPixelsPerEm, /* t0 8 */ T2K_TRANS_MATRIX *trans) {}

/* 00159670 00000090 */ void T2K_TransformXFunits(/* s2 18 */ T2K *t, /* a1 5 */ short int xValueInFUnits, /* s4 20 */ F16Dot16 *x, /* s3 19 */ F16Dot16 *y) {
	/* s1 17 */ F16Dot16 x16Dot16;
	/* s0 16 */ F16Dot16 y16Dot16;
	/* s0 16 */ F16Dot16 tmpX;
}

/* 00159700 0000008c */ void T2K_TransformYFunits(/* s1 17 */ T2K *t, /* a1 5 */ short int yValueInFUnits, /* s4 20 */ F16Dot16 *x, /* s3 19 */ F16Dot16 *y) {
	/* s2 18 */ F16Dot16 x16Dot16;
	/* s0 16 */ F16Dot16 y16Dot16;
}

/* 00159790 00000008 */ static void ConcatFontMatrix(/* a0 4 */ sfntClass *t, /* a1 5 */ T2K_TRANS_MATRIX *trans) {}

/* 00159798 000004cc */ void T2K_NewTransformation(/* s2 18 */ T2K *t, /* fp 30 */ int doSetUpNow, /* s4 20 */ int xRes, /* s0 16 */ int yRes, /* s3 19 */ T2K_TRANS_MATRIX *trans, /* t1 9 */ int enableSbits, /* t2 10 */ int *errCode) {
	/* s5 21 */ F16Dot16 xPointSize;
	/* s6 22 */ F16Dot16 yPointSize;
	/* s4 20 */ int xPixelsPerEm;
	/* s0 16 */ int yPixelsPerEm;
	/* s7 23 */ uint16 UPEM;
	/* 0x0(sp) */ T2K_FontWideMetrics hori;
	/* 0x20(sp) */ T2K_FontWideMetrics vert;
	/* v0 2 */ int usedOutlines;
}

/* 00159c68 00000034 */ static F26Dot6 scaleValue(/* a0 4 */ T2K *t, /* v0 2 */ F26Dot6 value, /* a2 6 */ int ppem) {}

/* 00159ca0 00000110 */ static void T2K_PurgeMemoryInternal(/* s0 16 */ T2K *t, /* s1 17 */ int level) {
	/* a1 5 */ int err;
}

/* 00159db0 00000020 */ void T2K_PurgeMemory(/* a0 4 */ T2K *t, /* a1 5 */ int level, /* a2 6 */ int *errCode) {}

/* 00159dd0 000013ac */ static void T2K_RenderGlyphInternal(/* s4 20 */ T2K *t, /* s0 16 */ int aCode, /* 0x94(sp) */ int depth, /* 0x98(sp) */ int8 xFracPenDelta, /* 0x9c(sp) */ int8 yFracPenDelta, /* 0xa0(sp) */ uint8 greyScaleLevel, /* 0xa4(sp) */ uint16 cmd) {
	/* 0xa8(sp) */ F26Dot6 *xPtr;
	/* 0xac(sp) */ F26Dot6 *yPtr;
	/* fp 30 */ int pointCount;
	/* s2 18 */ int i;
	/* s6 22 */ int n;
	/* s7 23 */ GlyphClass *glyph;
	/* 0xb0(sp) */ int isFigure;
	/* 0x30(sp) */ uint16 aw;
	/* 0x32(sp) */ uint16 ah;
	/* 0xb4(sp) */ uint16 UPEM;
	/* 0xb8(sp) */ int doNativeHints;
	/* 0xbc(sp) */ int doAutoHints;
	/* 0xc0(sp) */ short int agCmd;
	/* a0 4 */ int activate_droputcontrol;
	/* 0xc4(sp) */ int smart_droput;
	/* 0xc8(sp) */ int include_stubs;
	/* 0xcc(sp) */ int isTTfont;
	/* v1 3 */ int isStrokeFont;
	/* v1 3 */ int lowestRecPPEM;
	/* 0x90(sp) */ GlyphClass *base;
	/* s5 21 */ uint16 comp_cmd;
	/* s2 18 */ uint16 flags;
	/* 0xd0(sp) */ uint16 oredFlags;
	/* 0x40(sp) */ T2K_TRANS_MATRIX save;
	/* 0x50(sp) */ T2K_TRANS_MATRIX newbie;
	/* 0xd4(sp) */ int save_XPPEM;
	/* 0xd8(sp) */ int save_YPPEM;
	/* 0xdc(sp) */ short int *componentData;
	/* 0xe0(sp) */ uint8 *hintFragment;
	/* 0xe4(sp) */ int hintLength;
	/* s6 22 */ int gIndex;
	/* s1 17 */ int arg1;
	/* s3 19 */ int arg2;
	/* s1 17 */ F26Dot6 tmpX;
	/* s0 16 */ F26Dot6 tmpY;
	/* 0xe8(sp) */ F16Dot16 xMul;
	/* 0xec(sp) */ F16Dot16 yMul;
	/* s0 16 */ int err;
	/* 0x60(sp) */ ag_ElementType elem;
	/* s0 16 */ int err;
	/* 0x60(sp) */ ag_ElementType elem;
	/* t0 8 */ int xMicroPosition;
	/* a0 4 */ F26Dot6 error;
	/* a2 6 */ F26Dot6 x1Old;
	/* a1 5 */ F26Dot6 x1New;
	/* a3 7 */ F26Dot6 x2Old;
	/* v1 3 */ F26Dot6 x2New;
	/* v1 3 */ F26Dot6 wOld;
	/* v1 3 */ F26Dot6 wNew;
	/* v1 3 */ F26Dot6 xShift;
	/* fp 30 */ F16Dot16 t00;
	/* 0xf0(sp) */ F16Dot16 t01;
	/* 0xf4(sp) */ F16Dot16 t10;
	/* 0xf8(sp) */ F16Dot16 t11;
	/* s1 17 */ F26Dot6 tmpX;
	/* s3 19 */ F26Dot6 tmpY;
	/* t0 8 */ int maxSize;
	/* s5 21 */ ffStrkConv *sk;
	/* s3 19 */ tsiScanConv *sc;
	/* a2 6 */ char xDropOutControl;
	/* a3 7 */ char yDropOutControl;
	/* s1 17 */ uint16 gIndex;
	/* s1 17 */ int xRadius;
	/* s0 16 */ int yRadius;
	/* s0 16 */ int radius;
	/* s0 16 */ int extraColumn;
}

/* 0015b180 00000030 */ int32 T2K_GetNumAxes(/* a0 4 */ T2K *t) {
	/* a1 5 */ int32 numAxes;
}

/* 0015b1b0 00000008 */ F16Dot16 T2K_GetAxisGranularity(/* a0 4 */ T2K *t, /* a1 5 */ int32 n) {}

/* 0015b1b8 00000014 */ F16Dot16 T2K_GetCoordinate(/* a0 4 */ T2K *t, /* a1 5 */ int32 n) {
	/* v1 3 */ sfntClass *font;
}

/* 0015b1d0 00000014 */ void T2K_SetCoordinate(/* a0 4 */ T2K *t, /* a1 5 */ int32 n, /* a2 6 */ F16Dot16 value) {
	/* v0 2 */ sfntClass *font;
}

/* 0015b1e8 00000014 */ void T2K_GaspifyTheCmds(/* a0 4 */ T2K *t, /* a1 5 */ uint8 *greyScaleLevelPtr, /* a2 6 */ uint16 *cmdInPtr) {}

/* 0015b200 00000138 */ void T2K_RenderGlyph(/* s0 16 */ T2K *t, /* s2 18 */ int aCode, /* s3 19 */ int8 xFracPenDelta, /* s4 20 */ int8 yFracPenDelta, /* s1 17 */ uint8 greyScaleLevel, /* t1 9 */ uint16 cmdIn, /* t2 10 */ int *errCode) {
	/* 0x0(sp) */ uint16 cmd;
}

/* 0015b338 00000080 */ void T2K_SetNameString(/* s0 16 */ T2K *t, /* a1 5 */ uint16 languageID, /* a2 6 */ uint16 nameID, /* a3 7 */ int *errCode) {
	/* s1 17 */ sfntClass *font;
}

/* 0015b3b8 000000b0 */ void DeleteT2K(/* s0 16 */ T2K *t, /* a1 5 */ int *errCode) {}
/* 0015b468 00000024 */ uint16 T2K_GetGlyphIndex(/* a0 4 */ T2K *t, /* a1 5 */ uint16 charCode, /* a2 6 */ int *errCode) {}

/* 0015b490 0000013c */ uint32 T2K_MeasureTextInX(/* s4 20 */ T2K *t, /* a1 5 */ uint16 *text, /* a2 6 */ int16 *xKernValuesInFUnits, /* 0x0(sp) */ uint32 numChars) {
	/* 0x4(sp) */ uint32 i;
	/* fp 30 */ uint32 totalWidth;
	/* v0 2 */ uint32 thisWidth;
	/* s2 18 */ uint16 charCode;
	/* a0 4 */ uint16 prevCharCode;
	/* v1 3 */ uint32 cachePos;
	/* s0 16 */ uint32 cacheTag;
}

/* 0015b5d0 00000078 */ void T2K_GetIdealLineWidth(/* a0 4 */ T2K *t, /* a1 5 */ T2KCharInfo *cArr, /* a2 6 */ int *lineWidth, /* a3 7 */ T2KLayout *out) {
	/* a1 5 */ T2KCharInfo *cd;
	/* t3 11 */ int totalIntWidthX;
	/* t4 12 */ int totalIntWidthY;
	/* t1 9 */ F16Dot16 totSumX;
	/* t2 10 */ F16Dot16 totSumY;
}

/* 0015b648 00000370 */ void T2K_LayoutString(/* t5 13 */ T2KCharInfo *cArr, /* a1 5 */ int *LineWidthGoal, /* t4 12 */ T2KLayout *out) {
	/* s2 18 */ int i;
	/* v1 3 */ int j;
	/* t9 25 */ int MY_INDEX;
	/* t1 9 */ T2KCharInfo *cd;
	/* t3 11 */ int totalIntWidth;
	/* t0 8 */ F16Dot16 fracSum;
	/* s1 17 */ F16Dot16 spaceAdvance;
	/* a1 5 */ int error;
	/* a1 5 */ int goal;
	/* t7 15 */ int delta;
	/* s0 16 */ int deltaI;
	/* t6 14 */ int spaceCount;
	/* s5 21 */ int strLen;
	/* a0 4 */ int tmp;
	/* t2 10 */ int minSpace;
	/* t3 11 */ int maxSpace;
	/* a3 7 */ int mul;
	/* v0 2 */ int absError;
	/* v0 2 */ F16Dot16 scaleFactor;
}

/* 0015b9b8 00000030 */ void T2K_ListChars(/* a0 4 */ void *userArg, /* v0 2 */ T2K *scaler, /* v1 3 */ int (*ListCharsFn)(/* parameters unknown */), /* t0 8 */ int *errCode) {}
/* 0015b9e8 00000020 */ int FF_PSNameToCharCode(/* a0 4 */ T2K *t, /* a1 5 */ char *PSName, /* a2 6 */ int *errCode) {}

/* 0015ba08 000000cc */ uint8* FF_GetTTTablePointer(/* s2 18 */ T2K *t, /* a1 5 */ int tag, /* s3 19 */ unsigned char **ppTbl, /* s4 20 */ unsigned int *bufSize, /* s5 21 */ int *errCode) {
	/* s0 16 */ InputStream *in;
	/* s1 17 */ unsigned char *pOutBuf;
}

/* 0015bad8 00000034 */ void FF_ForceCMAPChange(/* s0 16 */ T2K *t, /* a1 5 */ int *errCode) {}

/* 0015bb10 0000004c */ int FF_GlyphExists(/* a0 4 */ T2K *t, /* a1 5 */ int code, /* a2 6 */ uint16 cmd, /* a3 7 */ int *errCode) {
	/* t0 8 */ int ret;
}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\strkconv.c
// *****************************************************************************

/* 0015bb60 000006c8 */ static void DrawLine(/* a0 4 */ ffStrkConv *t, /* s4 20 */ F26Dot6 x0, /* s5 21 */ F26Dot6 y0, /* s2 18 */ F26Dot6 x2, /* s3 19 */ F26Dot6 y2, /* 0x0(sp) */ int end0, /* 0x4(sp) */ int end2, /* 0x8(sp) */ int xRadius, /* 0xc0(sp) */ int yRadius) {
	/* 0xc(sp) */ F26Dot6 xA;
	/* t2 10 */ F26Dot6 xB;
	/* t1 9 */ F26Dot6 yA;
	/* 0x10(sp) */ F26Dot6 yB;
	/* t0 8 */ F26Dot6 center;
	/* t3 11 */ F26Dot6 dist;
	/* s6 22 */ F26Dot6 lim;
	/* s7 23 */ F26Dot6 limA;
	/* 0x14(sp) */ int endA;
	/* 0x18(sp) */ int endB;
	/* 0x1c(sp) */ int h1;
	/* 0x20(sp) */ int xmin;
	/* t9 25 */ int rowBytes;
	/* t4 12 */ uint8 *ptr;
	/* 0x24(sp) */ uint8 *baseAddr;
	/* a2 6 */ uint8 pixv;
	/* t6 14 */ uint8 pixv2;
	/* a1 5 */ uint8 pixo;
	/* t5 13 */ uint8 *ptrB;
	/* 0x28(sp) */ int mostlyX;
	/* s0 16 */ F26Dot6 absDx;
	/* s1 17 */ F26Dot6 absDy;
	/* t7 15 */ F26Dot6 z;
	/* v0 2 */ F26Dot6 y;
	/* a3 7 */ F26Dot6 yStep;
	/* t8 24 */ int done;
	/* a1 5 */ int y1;
	/* a2 6 */ int y2;
	/* a0 4 */ int x;
	/* a1 5 */ F26Dot6 z;
	/* a3 7 */ F26Dot6 x;
	/* t7 15 */ F26Dot6 xStep;
	/* t5 13 */ int done;
	/* v1 3 */ int x1;
	/* t1 9 */ int x2;
}

/* 0015c228 000001f8 */ static void DrawCurve(/* s2 18 */ ffStrkConv *t, /* t8 24 */ F26Dot6 x0, /* t7 15 */ F26Dot6 y0, /* t4 12 */ F26Dot6 x1, /* t0 8 */ F26Dot6 y1, /* t6 14 */ F26Dot6 x2, /* t5 13 */ F26Dot6 y2, /* t1 9 */ int end0, /* 0x240(sp) */ int end2, /* 0x248(sp) */ int xRadius, /* 0x250(sp) */ int yRadius) {
	/* s5 21 */ int end2;
	/* s4 20 */ int xRadius;
	/* s3 19 */ int yRadius;
	/* a2 6 */ int dZ;
	/* a3 7 */ int error;
	/* t2 10 */ int count;
	/* a0 4 */ int midX;
	/* a1 5 */ int midY;
	/* 0x10(sp) */ int Arr[112];
	/* s0 16 */ int *wp;
}

/* 0015c420 00000528 */ static void ParseStrokes(/* s7 23 */ ffStrkConv *t, /* 0x20(sp) */ int xRadius, /* 0x24(sp) */ int yRadius, /* a3 7 */ char omitBitMap, /* t0 8 */ FF_GetCacheMemoryPtr funcptr, /* t1 9 */ void *theCache, /* 0x28(sp) */ int bitRange255, /* 0x2c(sp) */ uint8 *remapBits) {
	/* t1 9 */ short int ctr;
	/* s0 16 */ short int point;
	/* fp 30 */ short int lastPoint;
	/* a2 6 */ int xmin;
	/* t5 13 */ int xmax;
	/* t2 10 */ int ymin;
	/* t4 12 */ int ymax;
	/* v1 3 */ int tmp;
	/* 0x30(sp) */ int *x;
	/* 0x34(sp) */ int *y;
	/* t6 14 */ uint8 *baseAddr;
	/* a1 5 */ int w;
	/* v0 2 */ int h;
	/* 0x38(sp) */ int N;
	/* a0 4 */ int i;
	/* v1 3 */ int lim;
	/* 0x3c(sp) */ char *onCurve;
	/* t0 8 */ F26Dot6 x0;
	/* a2 6 */ F26Dot6 y0;
	/* s5 21 */ F26Dot6 x2;
	/* s6 22 */ F26Dot6 y2;
	/* t5 13 */ int openContour;
	/* t1 9 */ int end0;
	/* t4 12 */ int end2;
	/* s2 18 */ F26Dot6 x4;
	/* s1 17 */ F26Dot6 y4;
	/* a1 5 */ int i;
	/* a0 4 */ uint8 tmp8;
}

/* 0015c948 0000009c */ ffStrkConv* ff_NewStrkConv(/* s1 17 */ tsiMemObject *mem, /* a1 5 */ short int numberOfContours, /* s6 22 */ short int *startPtr, /* s2 18 */ short int *endPtr, /* s3 19 */ int *xPtr, /* s4 20 */ int *yPtr, /* s5 21 */ char *onCurvePtr) {
	/* v1 3 */ ffStrkConv *t;
}

/* 0015c9e8 00000048 */ void MakeStrkBits(/* a0 4 */ ffStrkConv *t, /* a1 5 */ char omitBitMap, /* t6 14 */ FF_GetCacheMemoryPtr funcptr, /* v1 3 */ void *theCache, /* t4 12 */ int bitRange255, /* t5 13 */ uint8 *remapBits, /* t2 10 */ int xRadius, /* t3 11 */ int yRadius) {}
/* 0015ca30 00000074 */ void ff_DeleteStrkConv(/* s0 16 */ ffStrkConv *t) {}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\ghints.c
// *****************************************************************************

/* 0015caa8 0000009c */ static int GetYMax(/* t2 10 */ GlyphClass *glyph) {
	/* v1 3 */ int ctr;
	/* a2 6 */ int point;
	/* a1 5 */ int startPoint;
	/* a3 7 */ int endPoint;
	/* t6 14 */ short int *ooy;
	/* t1 9 */ short int ymax;
}

/* 0015cb48 0000009c */ static int GetYMin(/* t2 10 */ GlyphClass *glyph) {
	/* v1 3 */ int ctr;
	/* a2 6 */ int point;
	/* a1 5 */ int startPoint;
	/* a3 7 */ int endPoint;
	/* t6 14 */ short int *ooy;
	/* t1 9 */ short int ymin;
}

/* 0015cbe8 000000b4 */ void t2k_ReadGHints(/* s3 19 */ ag_GlobalDataType *gHints, /* s2 18 */ InputStream *in) {
	/* s3 19 */ int i;
}

/* 0015cca0 000001a4 */ static short int MedianHeight(/* s4 20 */ sfntClass *font, /* a1 5 */ unsigned char *s, /* 0x44(sp) */ int16 MaxOrMin) {
	/* v0 2 */ short int n;
	/* 0x48(sp) */ short int k;
	/* 0x0(sp) */ short int arr[32];
	/* s0 16 */ GlyphClass *glyph;
	/* 0x40(sp) */ uint16 aw;
	/* 0x42(sp) */ uint16 ah;
}

/* 0015ce48 00000530 */ void t2k_ComputeGlobalHints(/* s3 19 */ sfntClass *font, /* a1 5 */ ag_HintHandleType hintHandle, /* s2 18 */ ag_GlobalDataType *gHints, /* a3 7 */ int kanji) {
	/* a3 7 */ int i;
	/* a2 6 */ int j;
	/* 0x0(sp) */ short int arr[7];
	/* t4 12 */ short int median;
	/* t0 8 */ short int average;
	/* a1 5 */ short int tmp;
	/* a0 4 */ short int diffA;
	/* v1 3 */ short int diffB;
	/* v1 3 */ short int top;
	/* v0 2 */ short int bottom;
}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\ffstub.c
// *****************************************************************************

/* 0015d378 00000010 */ void FFInitStub(/* a0 4 */ ALLOC_FUNC allocptr, /* a1 5 */ FREE_FUNC freeptr, /* a2 6 */ REALLOC_FUNC reallocptr) {}
/* 0015d388 0000002c */ void* malloc_stub(/* a0 4 */ size_t s) {}
/* 0015d3b8 0000002c */ void free_stub(/* a0 4 */ void *ptr) {}
/* 0015d3e8 0000002c */ void* realloc_stub(/* a0 4 */ void *ptr, /* a1 5 */ size_t s) {}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\glyph.c
// *****************************************************************************

/* 0015d418 00000230 */ int ReadDeltaXYValue(/* t3 11 */ InputStream *in, /* t5 13 */ short int *dxPtr, /* t6 14 */ short int *dyPtr) {
	/* a2 6 */ int dx;
	/* a3 7 */ int dy;
	/* a0 4 */ int d1;
	/* a1 5 */ int d2;
	/* t4 12 */ int quadrant;
	/* a3 7 */ unsigned int w1;
}

/* 0015d648 00000008 */ void TEST_T2K_GLYPH(/* a0 4 */ tsiMemObject *mem) {}

/* 0015d650 000000e0 */ void glyph_AllocContours(/* s0 16 */ GlyphClass *t, /* a1 5 */ short int contourCountMax) {
	/* t0 8 */ short int ctr;
	/* s2 18 */ short int *sp;
	/* s1 17 */ short int *ep;
}

/* 0015d730 00000108 */ GlyphClass* New_EmptyGlyph(/* s1 17 */ tsiMemObject *mem, /* a1 5 */ int16 lsb, /* a2 6 */ uint16 aw, /* a3 7 */ int16 tsb, /* t0 8 */ uint16 ah) {
	/* a0 4 */ short int *oox;
	/* a1 5 */ short int *ooy;
	/* s0 16 */ GlyphClass *t;
}

/* 0015d838 000004c0 */ GlyphClass* New_GlyphClassT2K(/* s0 16 */ tsiMemObject *mem, /* s4 20 */ InputStream *in, /* a2 6 */ char readHints, /* 0x4(sp) */ int16 lsb, /* 0x8(sp) */ uint16 aw, /* 0xc(sp) */ int16 tsb, /* 0x10(sp) */ uint16 ah, /* t3 11 */ void *model) {
	/* s2 18 */ int i;
	/* 0x14(sp) */ short int *oox;
	/* 0x18(sp) */ short int *ooy;
	/* 0x1c(sp) */ uint8 *onCurve;
	/* 0x20(sp) */ int pointCount;
	/* s1 17 */ GlyphClass *t;
	/* fp 30 */ short int xmin;
	/* s7 23 */ short int ymax;
	/* s3 19 */ short int flags;
	/* s2 18 */ short int *componentData;
	/* s0 16 */ int componentSize;
	/* s5 21 */ short int x;
	/* s6 22 */ short int y;
	/* 0x24(sp) */ short int stmp;
	/* 0x0(sp) */ short int dx;
	/* 0x2(sp) */ short int dy;
	/* a0 4 */ int xMid;
}

/* 0015dcf8 000004a8 */ void Add_GlyphClass(/* t3 11 */ GlyphClass **tPtr, /* s2 18 */ GlyphClass *addMe, /* s4 20 */ uint16 flags, /* s5 21 */ int arg1, /* s6 22 */ int arg2) {
	/* t1 9 */ int i;
	/* t2 10 */ int j;
	/* s1 17 */ GlyphClass *t;
	/* s3 19 */ int pointCount;
	/* s0 16 */ int n;
	/* s7 23 */ int contourCount;
	/* 0x0(sp) */ short int *sp;
	/* 0x4(sp) */ short int *ep;
	/* 0x8(sp) */ short int *oox;
	/* 0xc(sp) */ short int *ooy;
	/* s0 16 */ uint8 *onCurve;
	/* 0x10(sp) */ F26Dot6 *x;
	/* fp 30 */ F26Dot6 *y;
	/* t0 8 */ F26Dot6 xDelta;
	/* a3 7 */ F26Dot6 yDelta;
}

/* 0015e1a0 00000080 */ void AllocGlyphPointMemory(/* s1 17 */ GlyphClass *t, /* s2 18 */ int pointCountMax) {
	/* s0 16 */ int n;
}

/* 0015e220 000000b8 */ void Delete_GlyphClass(/* s0 16 */ GlyphClass *t) {}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\truetype.c
// *****************************************************************************

/* 0015e2d8 000000e8 */ void setT2KScaleFactors(/* a0 4 */ int pixelsPerEm, /* a1 5 */ int UPEM, /* s2 18 */ T2KScaleInfo *si) {
	/* s0 16 */ int32 nScale;
	/* a0 4 */ int16 dShift;
	/* s1 17 */ int32 dScale;
	/* s3 19 */ int32 dScaleDiv2;
}

/* 0015e3c0 00000038 */ static int IsFigure_cmapClass(/* a0 4 */ cmapClass *t, /* a1 5 */ uint16 gIndex) {
	/* v1 3 */ int i;
}

/* 0015e3f8 0000010c */ static uint16 Compute_cmapClass_Index9999(/* a0 4 */ cmapClass *t, /* a1 5 */ uint16 charCode) {
	/* a1 5 */ int i;
	/* a1 5 */ int j;
	/* t0 8 */ int glyphCount;
	/* t2 10 */ uint16 index;
	/* t1 9 */ uint16 baseIndex;
	/* a3 7 */ uint8 *charP;
	/* v0 2 */ uint8 *ptr;
	/* a1 5 */ uint8 lowByte;
	/* a2 6 */ uint8 x;
}

/* 0015e508 00000050 */ static uint16 Compute_cmapClass_Index0(/* a2 6 */ cmapClass *t, /* a1 5 */ uint16 charCode) {
	/* a1 5 */ uint16 index;
	/* v1 3 */ uint8 *charP;
}

/* 0015e558 00000170 */ static uint16 Compute_cmapClass_Index2(/* s2 18 */ cmapClass *t, /* s5 21 */ uint16 charCode) {
	/* s6 22 */ uint16 index;
	/* s0 16 */ uint8 *charP;
	/* s1 17 */ uint16 mapMe;
	/* a0 4 */ uint16 highByte;
	/* s7 23 */ uint16 lowByte;
	/* s3 19 */ sfnt_subHeader *subHeader;
	/* s0 16 */ uint16 *shortP;
}

/* 0015e6c8 00000178 */ static uint16 Compute_cmapClass_Index4(/* t4 12 */ cmapClass *t, /* t3 11 */ uint16 charCode) {
	/* t2 10 */ uint16 segCountX2;
	/* t0 8 */ uint16 offset;
	/* a3 7 */ uint16 idDelta;
	/* a0 4 */ uint16 index;
	/* a2 6 */ uint16 uTmp;
	/* a1 5 */ uint8 *charP;
	/* t1 9 */ uint16 n0;
	/* a3 7 */ uint16 n1;
	/* t0 8 */ uint16 n2;
}

/* 0015e840 00000098 */ static uint16 Compute_cmapClass_Index6(/* t0 8 */ cmapClass *t, /* a1 5 */ uint16 charCode) {
	/* a3 7 */ uint16 index;
	/* a2 6 */ uint8 *charP;
}

/* 0015e8d8 000000a4 */ static uint16 Compute_cmapClass_GlyphIndex(/* a0 4 */ cmapClass *t, /* a1 5 */ uint16 charCode) {
	/* a2 6 */ uint16 gIndex;
}

/* 0015e980 00000398 */ static cmapClass* New_cmapClass(/* s4 20 */ tsiMemObject *mem, /* s6 22 */ uint16 preferedPlatformID, /* s5 21 */ uint16 preferedPlatformSpecificID, /* s3 19 */ InputStream *in) {
	/* s2 18 */ int i;
	/* t0 8 */ int pass;
	/* a2 6 */ uint16 format;
	/* a1 5 */ uint8 *charP;
	/* s1 17 */ cmapClass *t;
	/* a0 4 */ int A;
	/* v1 3 */ int B;
	/* a1 5 */ char c;
}

/* 0015ed18 00000088 */ static void Delete_cmapClass(/* s1 17 */ cmapClass *t) {
	/* s0 16 */ int i;
}

/* 0015eda0 00000078 */ static sfnt_DirectoryEntry* New_sfnt_DirectoryEntry(/* s2 18 */ tsiMemObject *mem, /* s0 16 */ InputStream *in) {
	/* s1 17 */ sfnt_DirectoryEntry *t;
}

/* 0015ee18 00000020 */ static void Delete_sfnt_DirectoryEntry(/* a1 5 */ sfnt_DirectoryEntry *t) {}

/* 0015ee38 000000dc */ sfnt_OffsetTable* New_sfnt_OffsetTable(/* s3 19 */ tsiMemObject *mem, /* s2 18 */ InputStream *in) {
	/* s1 17 */ int i;
	/* s0 16 */ sfnt_OffsetTable *t;
}

/* 0015ef18 00000078 */ void Delete_sfnt_OffsetTable(/* s1 17 */ sfnt_OffsetTable *t) {
	/* s0 16 */ int i;
}

/* 0015ef90 00000114 */ static hheaClass* New_hheaClass(/* s2 18 */ tsiMemObject *mem, /* s0 16 */ InputStream *in) {
	/* s1 17 */ hheaClass *t;
}

/* 0015f0a8 00000024 */ static void Delete_hheaClass(/* a1 5 */ hheaClass *t) {}

/* 0015f0d0 000000a4 */ hmtxClass* New_hmtxEmptyClass(/* s2 18 */ tsiMemObject *mem, /* s3 19 */ int32 numGlyphs, /* s0 16 */ int32 numberOfHMetrics) {
	/* s1 17 */ hmtxClass *t;
}

/* 0015f178 000000f8 */ static hmtxClass* New_hmtxClass(/* a0 4 */ tsiMemObject *mem, /* s4 20 */ InputStream *in, /* s5 21 */ int32 numGlyphs, /* s3 19 */ int32 numberOfHMetrics) {
	/* s1 17 */ int32 i;
	/* s3 19 */ uint16 last_aw;
	/* s2 18 */ hmtxClass *t;
}

/* 0015f270 00000048 */ void Delete_hmtxClass(/* s0 16 */ hmtxClass *t) {}

/* 0015f2b8 000000dc */ static ttcfClass* New_ttcfClass(/* s3 19 */ tsiMemObject *mem, /* s1 17 */ InputStream *in) {
	/* s2 18 */ ttcfClass *t;
	/* s0 16 */ uint32 i;
}

/* 0015f398 0000003c */ static void Delete_ttcfClass(/* s0 16 */ ttcfClass *t) {}

/* 0015f3d8 0000012c */ static headClass* New_headClass(/* s2 18 */ tsiMemObject *mem, /* s0 16 */ InputStream *in) {
	/* s1 17 */ headClass *t;
}

/* 0015f508 00000024 */ static void Delete_headClass(/* a1 5 */ headClass *t) {}

/* 0015f530 0000011c */ static maxpClass* New_maxpClass(/* s0 16 */ tsiMemObject *mem, /* s2 18 */ InputStream *in) {
	/* s1 17 */ maxpClass *t;
}

/* 0015f650 00000024 */ static void Delete_maxpClass(/* a1 5 */ maxpClass *t) {}

/* 0015f678 0000013c */ locaClass* New_locaClass(/* s4 20 */ tsiMemObject *mem, /* s5 21 */ InputStream *in, /* s2 18 */ short int indexToLocFormat, /* s0 16 */ int length) {
	/* s0 16 */ int i;
	/* s1 17 */ int n;
	/* s3 19 */ locaClass *t;
}

/* 0015f7b8 0000003c */ static void Delete_locaClass(/* s0 16 */ locaClass *t) {}

/* 0015f7f8 000000e0 */ slocClass* FF_New_slocClass(/* s0 16 */ tsiMemObject *mem, /* s2 18 */ InputStream *in) {
	/* s3 19 */ int i;
	/* v1 3 */ T2K_sloc_entry *sloc;
	/* s1 17 */ slocClass *t;
}

/* 0015f8d8 00000130 */ uint32 FF_SLOC_MapIndexToOffset(/* s3 19 */ slocClass *t, /* s0 16 */ InputStream *in, /* s1 17 */ int32 gIndex) {
	/* a3 7 */ uint16 _gIndexA;
	/* v1 3 */ uint16 _gIndexB;
	/* v0 2 */ int32 estimate;
	/* a0 4 */ int low;
	/* a1 5 */ int high;
	/* s2 18 */ int mid;
	/* s4 20 */ T2K_sloc_entry *sloc;
}

/* 0015fa08 0000003c */ void FF_Delete_slocClass(/* s0 16 */ slocClass *t) {}

/* 0015fa48 000006d8 */ static GlyphClass* New_GlyphClass(/* s0 16 */ tsiMemObject *mem, /* s2 18 */ InputStream *in, /* s7 23 */ char readHints, /* 0x0(sp) */ int16 lsb, /* 0x4(sp) */ uint16 aw, /* 0x8(sp) */ int16 tsb, /* 0xc(sp) */ uint16 ah) {
	/* s1 17 */ int i;
	/* a3 7 */ short int *oox;
	/* t1 9 */ short int *ooy;
	/* t0 8 */ uint8 *onCurve;
	/* s6 22 */ int pointCount;
	/* s5 21 */ int contourCount;
	/* s3 19 */ GlyphClass *t;
	/* s4 20 */ short int flags;
	/* s5 21 */ int weHaveInstructions;
	/* s1 17 */ short int *componentData;
	/* s0 16 */ int componentSize;
	/* a1 5 */ uint8 flag;
	/* s4 20 */ short int stmp;
	/* a0 4 */ int j;
	/* a0 4 */ int xMid;
}

/* 00160120 00000028 */ uint16 GetUPEM(/* a0 4 */ sfntClass *t) {
	/* v1 3 */ uint16 upem;
}

/* 00160148 00000044 */ sfnt_DirectoryEntry* GetTableDirEntry_sfntClass(/* a0 4 */ sfntClass *t, /* a1 5 */ int tag) {
	/* a2 6 */ int i;
}

/* 00160190 00000058 */ InputStream* GetStreamForTable(/* s0 16 */ sfntClass *t, /* a1 5 */ int tag) {
	/* s1 17 */ InputStream *stream;
	/* v0 2 */ sfnt_DirectoryEntry *dirEntry;
}

/* 001601e8 00000628 */ static void CacheKeyTables_sfntClass(/* s1 17 */ sfntClass *t, /* s3 19 */ InputStream *in, /* s0 16 */ int32 logicalFontNumber) {
	/* s2 18 */ InputStream *stream;
	/* s0 16 */ sfnt_DirectoryEntry *dirEntry;
}

/* 00160810 000002a8 */ void GetTTNameProperty(/* s4 20 */ sfntClass *font, /* 0x0(sp) */ uint16 languageID, /* 0x4(sp) */ uint16 nameID, /* s7 23 */ uint8 **p8, /* s6 22 */ uint16 **p16) {
	/* s2 18 */ InputStream *stream;
	/* v0 2 */ sfnt_DirectoryEntry *dirEntry;
	/* s4 20 */ uint8 *p;
	/* 0x8(sp) */ uint16 platFormID;
	/* 0xc(sp) */ uint16 encodingID;
	/* s0 16 */ InputStream *in;
	/* s3 19 */ uint16 n;
	/* s1 17 */ uint16 i;
	/* s0 16 */ uint32 offset;
	/* s5 21 */ uint16 stringStorageOffset;
	/* v0 2 */ uint16 stringOffset;
	/* s3 19 */ uint16 stringLength;
	/* fp 30 */ int is16Bit;
}

/* 00160ab8 00000028 */ int GetNumGlyphs_sfntClass(/* a0 4 */ sfntClass *t) {
	/* a0 4 */ int n;
}

/* 00160ae0 000002a8 */ GlyphClass* GetGlyphByIndex(/* s0 16 */ sfntClass *t, /* s3 19 */ int index, /* 0x0(sp) */ char readHints, /* s7 23 */ uint16 *aWidth, /* fp 30 */ uint16 *aHeight) {
	/* s0 16 */ GlyphClass *glyph;
	/* t0 8 */ sfnt_DirectoryEntry *dirEntry;
	/* s1 17 */ InputStream *stream;
	/* t1 9 */ unsigned int offset1;
	/* a2 6 */ unsigned int offset2;
	/* a3 7 */ unsigned int length;
	/* s6 22 */ uint16 aw;
	/* s2 18 */ uint16 ah;
	/* s4 20 */ int16 lsb;
	/* s5 21 */ int16 tsb;
}

/* 00160d88 00000090 */ void ff_LoadCMAP(/* s1 17 */ sfntClass *t) {
	/* s0 16 */ InputStream *stream;
	/* v0 2 */ sfnt_DirectoryEntry *dirEntry;
}

/* 00160e18 00000008 */ uint16 SfntClassPSNameTocharCode(/* a0 4 */ sfntClass *t, /* a1 5 */ char *PSName) {}
/* 00160e20 00000044 */ uint16 GetSfntClassGlyphIndex(/* s0 16 */ sfntClass *t, /* s1 17 */ uint16 charCode) {}
/* 00160e68 0000003c */ int IsFigure(/* s0 16 */ sfntClass *t, /* a1 5 */ uint16 gIndex) {}

/* 00160ea8 00000050 */ static void SetStyling(/* a0 4 */ sfntClass *t, /* a1 5 */ T2K_AlgStyleDescriptor *styling) {
	/* t0 8 */ int i;
}

/* 00160ef8 00000018 */ static uint16 Get_hmtx_AW(/* a0 4 */ void *param1, /* a1 5 */ uint16 gIndex) {}

/* 00160f10 000000f4 */ static uint16 Get_Cached_AW(/* s0 16 */ void *param1, /* s2 18 */ uint16 gIndex) {
	/* s1 17 */ uint16 key;
	/* v1 3 */ int i;
	/* v0 2 */ uint16 *p;
}

/* 00161008 00000008 */ static uint16 Get_Upem_Width(/* a0 4 */ void *param1, /* a1 5 */ uint16 gIndex) {}

/* 00161010 0000022c */ sfntClass* FF_New_sfntClass(/* s1 17 */ tsiMemObject *mem, /* s2 18 */ short int fontType, /* s6 22 */ int32 fontNum, /* s4 20 */ InputStream *in1, /* s5 21 */ InputStream *in2, /* s3 19 */ T2K_AlgStyleDescriptor *styling, /* t2 10 */ int *errCode) {
	/* s0 16 */ sfntClass *t;
}

/* 00161240 00000028 */ int GetMaxPoints(/* a0 4 */ sfntClass *t) {
	/* v0 2 */ int result;
}

/* 00161268 000001d8 */ void GetFontWideOutlineMetrics(/* t0 8 */ sfntClass *font, /* a1 5 */ T2K_FontWideMetrics *hori, /* a2 6 */ T2K_FontWideMetrics *vert) {
	/* t1 9 */ int i;
}

/* 00161440 0000002c */ void Purge_cmapMemory(/* s0 16 */ sfntClass *t) {}
/* 00161470 000000b0 */ void FF_Delete_sfntClass(/* s0 16 */ sfntClass *t, /* a1 5 */ int *errCode) {}

/* 00161520 000000ac */ void ff_KernShellSort(/* a0 4 */ kernPair0Struct *pairs, /* a1 5 */ int num_pair) {
	/* v1 3 */ int i;
	/* t0 8 */ int j;
	/* t2 10 */ int incr;
}

/* 001615d0 0000001c */ static int16 Read2B(/* a0 4 */ void *p) {}

/* 001615f0 00000594 */ static int Read_PreferredMappingTable(/* 0x10(sp) */ void *userArg, /* s7 23 */ cmapClass *t, /* 0x14(sp) */ void *ctxPtr, /* 0x18(sp) */ int (*ListCharsFn)(/* parameters unknown */)) {
	/* s1 17 */ uint16 ii;
	/* s2 18 */ uint16 jj;
	/* s1 17 */ uint16 kk;
	/* s3 19 */ uint16 nr;
	/* s3 19 */ uint16 *cmapPtr;
	/* s5 21 */ uint16 *endCount;
	/* 0x1c(sp) */ uint16 *startCount;
	/* fp 30 */ uint16 *idDelta;
	/* 0x20(sp) */ uint16 *idOffset;
	/* a0 4 */ uint16 glyphIndex;
	/* s4 20 */ rangeCount *charRange;
	/* s0 16 */ subHeaders *sub_header;
	/* 0x0(sp) */ subHeaders subRange;
	/* s6 22 */ int checkStop;
	/* fp 30 */ uint16 *glyphIds;
	/* s0 16 */ uint8 *p;
	/* s3 19 */ uint16 glyphCode;
	/* s0 16 */ uint8 *bPtr;
	/* s1 17 */ uint8 mask;
}

/* 00161b88 00000068 */ void T2K_SfntListChars(/* s1 17 */ void *userArg, /* s0 16 */ sfntClass *t, /* s2 18 */ void *ctxPtr, /* s3 19 */ int (*ListCharsFn)(/* parameters unknown */), /* t0 8 */ int *errCode) {}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\tsimem.c
// *****************************************************************************

/* 00161bf0 00000010 */ void tsi_Error(/* a0 4 */ tsiMemObject *t, /* a1 5 */ int errcode) {}

/* 00161c00 00000110 */ tsiMemObject* tsi_NewMemhandler(/* s1 17 */ int *errCode) {
	/* s0 16 */ tsiMemObject *t;
	/* a0 4 */ int i;
}

/* 00161d10 000000b4 */ void* tsi_FastAllocN(/* s1 17 */ tsiMemObject *t, /* s2 18 */ unsigned int size, /* a2 6 */ int N) {
	/* v0 2 */ void *p;
}

/* 00161dc8 00000068 */ void tsi_FreeFastMemBlocks(/* s2 18 */ tsiMemObject *t) {
	/* s1 17 */ int i;
}

/* 00161e30 00000034 */ void tsi_DeleteMemhandler(/* s0 16 */ tsiMemObject *t) {}

/* 00161e68 00000080 */ void tsi_EmergencyShutDown(/* s2 18 */ tsiMemObject *t) {
	/* s1 17 */ int i;
	/* v0 2 */ int maxPointers;
	/* s3 19 */ void **base;
}

/* 00161ee8 0000012c */ void* tsi_AllocMem(/* s0 16 */ tsiMemObject *t, /* s2 18 */ unsigned int size) {
	/* a2 6 */ int i;
	/* a1 5 */ int maxPointers;
	/* s1 17 */ unsigned char *p;
	/* a3 7 */ void **base;
	/* a0 4 */ unsigned int index;
}

/* 00162018 00000158 */ void* tsi_ReAllocMem(/* s1 17 */ tsiMemObject *t, /* a1 5 */ void *pIn, /* s3 19 */ unsigned int size2) {
	/* s4 20 */ int i;
	/* v0 2 */ int maxPointers;
	/* s2 18 */ void **base;
	/* s0 16 */ unsigned char *p;
	/* v0 2 */ unsigned int size1;
}

/* 00162170 000000cc */ void tsi_DeAllocMem(/* s1 17 */ tsiMemObject *t, /* v0 2 */ void *pIn) {
	/* a0 4 */ void **base;
	/* s0 16 */ char *p;
	/* v0 2 */ unsigned int size;
	/* v0 2 */ unsigned int index;
}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\t2kstrm.c
// *****************************************************************************

/* 00162240 00000048 */ int32 ReadInt32(/* a0 4 */ InputStream *t) {
	/* v1 3 */ unsigned char *ptr;
	/* v0 2 */ unsigned int pos;
	/* v0 2 */ uint32 lword;
}

/* 00162288 00000030 */ int16 ReadInt16(/* a0 4 */ InputStream *t) {
	/* a1 5 */ unsigned char *ptr;
	/* v0 2 */ unsigned int pos;
}

/* 001622b8 000000b4 */ void ReadSegment(/* s2 18 */ InputStream *t, /* a0 4 */ uint8 *dest, /* s0 16 */ int numBytes) {
	/* s1 17 */ unsigned int pos;
	/* a2 6 */ int numBytes2;
	/* a1 5 */ int i;
}

/* 00162370 00000010 */ int SizeInStream(/* a0 4 */ InputStream *stream) {}
/* 00162380 00000010 */ unsigned char* GetEntireStreamIntoMemory(/* a0 4 */ InputStream *stream) {}

/* 00162390 00000074 */ InputStream* New_InputStream(/* s0 16 */ tsiMemObject *mem, /* s1 17 */ unsigned char *dataPtr, /* s2 18 */ unsigned int length, /* a3 7 */ int *errCode) {
	/* v1 3 */ InputStream *t;
}

/* 00162408 000000ac */ InputStream* New_InputStream2(/* s0 16 */ tsiMemObject *mem, /* s1 17 */ InputStream *in, /* s2 18 */ unsigned int offset, /* s3 19 */ unsigned int length, /* t0 8 */ int fastIndex, /* t1 9 */ int *errCode) {
	/* t0 8 */ InputStream *t;
}

/* 001624b8 00000074 */ InputStream* New_InputStream3(/* s0 16 */ tsiMemObject *mem, /* s1 17 */ unsigned char *dataPtr, /* s2 18 */ unsigned int length, /* a3 7 */ int *errCode) {
	/* v1 3 */ InputStream *t;
}

/* 00162530 0000000c */ void Rewind_InputStream(/* a0 4 */ InputStream *t) {}
/* 00162540 00000010 */ void Seek_InputStream(/* a0 4 */ InputStream *t, /* a1 5 */ uint32 offset) {}
/* 00162550 00000010 */ uint32 Tell_InputStream(/* a0 4 */ InputStream *t) {}
/* 00162560 00000070 */ void Delete_InputStream(/* s0 16 */ InputStream *t, /* a1 5 */ int *errCode) {}

/* 001625d0 00000044 */ unsigned int ReadUnsignedNumber(/* a1 5 */ InputStream *in) {
	/* a3 7 */ unsigned int n;
	/* a2 6 */ unsigned int shift;
}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\t2ksc.c
// *****************************************************************************

/* 00162618 00000138 */ static T2KInterSectType* AllocFreeNodes(/* s0 16 */ tsiScanConv *t) {
	/* a0 4 */ T2KInterSectType *ptr;
	/* s2 18 */ int N;
	/* a2 6 */ int i;
}

/* 00162750 00000080 */ static void FreeAllNodes(/* s0 16 */ tsiScanConv *t) {
	/* s1 17 */ int i;
}

/* 001627d0 000002d8 */ static void ComputeScanBBox(/* t2 10 */ tsiScanConv *t) {
	/* v1 3 */ int i;
	/* t0 8 */ int first;
	/* a3 7 */ int last;
	/* t5 13 */ int xmin;
	/* t4 12 */ int xmax;
	/* t6 14 */ int ymin;
	/* t3 11 */ int ymax;
	/* a1 5 */ int x;
	/* a1 5 */ int y;
	/* a0 4 */ T2KInterSectType *next;
	/* a2 6 */ T2KInterSectType *node;
}

/* 00162aa8 00000084 */ static void ComputeOutlineBBox(/* a0 4 */ tsiScanConv *t, /* a1 5 */ int *xPtr, /* a2 6 */ int *yPtr, /* a3 7 */ int lastPoint) {
	/* t1 9 */ int i;
	/* a1 5 */ int *ptr;
	/* t3 11 */ int min;
	/* t4 12 */ int max;
	/* v1 3 */ int tmp;
	/* t5 13 */ int direction;
}

/* 00162b30 00000650 */ static void drawLine(/* s5 21 */ tsiScanConv *t, /* 0x0(sp) */ int x0, /* 0x4(sp) */ int y0, /* 0x8(sp) */ int x2, /* fp 30 */ int y2) {
	/* 0xc(sp) */ int xA;
	/* 0x10(sp) */ int xB;
	/* 0x14(sp) */ int yA;
	/* 0x18(sp) */ int yB;
	/* s2 18 */ int center;
	/* 0x1c(sp) */ int dist;
	/* 0x20(sp) */ uint8 flagBit0;
	/* s6 22 */ T2KInterSectType **head;
	/* s0 16 */ uint32 utmp32;
	/* s3 19 */ int y;
	/* s7 23 */ int yStep;
	/* s3 19 */ T2KInterSectType **headPtr;
	/* s1 17 */ T2KInterSectType *node;
	/* a1 5 */ T2KInterSectType *newbie;
	/* v1 3 */ T2KInterSectType *next;
	/* s4 20 */ T2KInterSectType **headPtr;
	/* s1 17 */ T2KInterSectType *node;
	/* a2 6 */ T2KInterSectType *newbie;
	/* a1 5 */ T2KInterSectType *next;
	/* s3 19 */ int x;
	/* s7 23 */ int xStep;
	/* s3 19 */ T2KInterSectType **headPtr;
	/* s1 17 */ T2KInterSectType *node;
	/* a1 5 */ T2KInterSectType *newbie;
	/* v1 3 */ T2KInterSectType *next;
	/* s4 20 */ T2KInterSectType **headPtr;
	/* s1 17 */ T2KInterSectType *node;
	/* a2 6 */ T2KInterSectType *newbie;
	/* s0 16 */ int32 coordinate25Dot6_flag1;
	/* a1 5 */ T2KInterSectType *next;
}

/* 00163180 000001a4 */ static void drawParabola(/* s1 17 */ tsiScanConv *t, /* t6 14 */ int x0, /* t5 13 */ int y0, /* t3 11 */ int x1, /* t0 8 */ int y1, /* t1 9 */ int x2, /* t2 10 */ int y2) {
	/* a3 7 */ int dZ;
	/* a2 6 */ int error;
	/* t4 12 */ int count;
	/* a0 4 */ int midX;
	/* a1 5 */ int midY;
	/* 0x0(sp) */ int Arr[112];
	/* s0 16 */ int *wp;
}

/* 00163328 000002ec */ static void Make2ndDegreeEdgeList(/* t4 12 */ tsiScanConv *t) {
	/* s6 22 */ int startPoint;
	/* t5 13 */ int lastPoint;
	/* t1 9 */ int ctr;
	/* a3 7 */ int ptA;
	/* s4 20 */ int ptB;
	/* s0 16 */ int ptC;
	/* t0 8 */ int Ax;
	/* s7 23 */ int Bx;
	/* s3 19 */ int Cx;
	/* a2 6 */ int Ay;
	/* fp 30 */ int By;
	/* s5 21 */ int Cy;
	/* t7 15 */ int *x;
	/* t6 14 */ int *y;
	/* t9 25 */ char *onCurve;
	/* s1 17 */ int tmpX;
	/* s2 18 */ int tmpY;
}

/* 00163618 000002d4 */ tsiScanConv* tsi_NewScanConv(/* s6 22 */ tsiMemObject *mem, /* 0x0(sp) */ short int numberOfContours, /* 0x4(sp) */ short int *startPtr, /* fp 30 */ short int *endPtr, /* 0x8(sp) */ int *xPtr, /* 0xc(sp) */ int *yPtr, /* 0x10(sp) */ char *onCurvePtr, /* s3 19 */ uint8 greyScaleLevel, /* 0xc0(sp) */ int curveType, /* 0xc8(sp) */ int xDropOutControl, /* 0xd0(sp) */ int yDropOutControl, /* 0xd8(sp) */ int smart_droput, /* 0xe0(sp) */ int include_stubs, /* 0xe8(sp) */ F26Dot6 oneHalfFUnit) {
	/* 0x14(sp) */ char curveType;
	/* s5 21 */ char xDropOutControl;
	/* s4 20 */ char yDropOutControl;
	/* s0 16 */ int smart_droput;
	/* s1 17 */ int include_stubs;
	/* s7 23 */ F26Dot6 oneHalfFUnit;
	/* s2 18 */ tsiScanConv *t;
	/* v0 2 */ int i;
	/* s1 17 */ int min;
	/* s3 19 */ int max;
	/* s0 16 */ int tmp;
	/* s4 20 */ int direction;
	/* a0 4 */ T2KInterSectType **base;
}

/* 001638f0 00000b50 */ void MakeBits(/* s0 16 */ tsiScanConv *t, /* a1 5 */ char xWeightIsOne, /* s4 20 */ char omitBitMap, /* s6 22 */ FF_GetCacheMemoryPtr funcptr, /* 0x0(sp) */ void *theCache, /* 0x4(sp) */ int bitRange255, /* 0x8(sp) */ uint8 *remapBits, /* s1 17 */ int extraColumn) {
	/* a3 7 */ int i;
	/* fp 30 */ int N;
	/* s5 21 */ int xmin;
	/* v1 3 */ int xmax;
	/* s2 18 */ int ymin;
	/* t9 25 */ int ymax;
	/* s7 23 */ int w;
	/* s1 17 */ int h;
	/* s3 19 */ int rowBytes;
	/* s4 20 */ unsigned char *baseAddr;
	/* t4 12 */ uint8 *ptr;
	/* 0xc(sp) */ int xmid;
	/* 0x10(sp) */ int ymid;
	/* 0x14(sp) */ uint8 greyScaleLevel;
	/* t3 11 */ int windingCount;
	/* v1 3 */ int lim;
	/* 0x18(sp) */ int h1;
	/* v0 2 */ T2KInterSectType *node;
	/* t0 8 */ T2KInterSectType *next;
	/* a1 5 */ int x1;
	/* a2 6 */ int x2;
	/* a0 4 */ int xA;
	/* a3 7 */ int xB;
	/* v0 2 */ T2KInterSectType *node;
	/* t5 13 */ T2KInterSectType *next;
	/* t1 9 */ int y1;
	/* t0 8 */ int y2;
	/* a1 5 */ int x;
	/* t2 10 */ int k;
	/* t4 12 */ int yA;
	/* a3 7 */ int yB;
	/* a2 6 */ uint8 otherSample;
	/* t7 15 */ uint8 thisSample;
	/* t6 14 */ uint8 *otherPtr;
	/* v0 2 */ int w1;
	/* v1 3 */ int w2;
	/* v1 3 */ int w1;
	/* a0 4 */ int w2;
	/* v1 3 */ int w1;
	/* v0 2 */ int w2;
	/* v0 2 */ int tmp8;
	/* v0 2 */ T2KInterSectType *node;
	/* t2 10 */ T2KInterSectType *next;
	/* t0 8 */ int x1;
	/* a3 7 */ int x2;
	/* a2 6 */ int b1;
	/* t1 9 */ int b2;
	/* fp 30 */ int weDidXDropouts;
	/* v1 3 */ T2KInterSectType *node;
	/* t0 8 */ T2KInterSectType *next;
	/* a2 6 */ int x1;
	/* a3 7 */ int x2;
	/* a0 4 */ int x;
	/* t1 9 */ int k;
	/* t7 15 */ int weDidYDropouts;
	/* v0 2 */ T2KInterSectType *node;
	/* t0 8 */ T2KInterSectType *next;
	/* a2 6 */ int y1;
	/* a1 5 */ int y2;
	/* a0 4 */ int y;
	/* t2 10 */ int x;
	/* v0 2 */ int k1;
	/* a1 5 */ int k2;
}

/* 00164440 000000ac */ void tsi_DeleteScanConv(/* s0 16 */ tsiScanConv *t) {}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\t2ktt.c
// *****************************************************************************

/* 001644f0 00000434 */ T2KTTClass* New_T2KTTClass(/* fp 30 */ tsiMemObject *mem, /* 0x0(sp) */ InputStream *in, /* 0x4(sp) */ void *fontPtr) {
	/* s3 19 */ T2KTTClass *t;
	/* s5 21 */ fnt_GlobalGraphicStateType *g;
	/* s0 16 */ sfnt_DirectoryEntry *dirEntry;
	/* s2 18 */ int i;
	/* s1 17 */ int n;
	/* 0x8(sp) */ maxpClass *maxp;
	/* s0 16 */ int16 *ocvt;
	/* s2 18 */ InputStream *stream;
	/* v0 2 */ F26Dot6 *ptr32;
	/* v1 3 */ int16 *ptr16;
	/* v0 2 */ uint8 *ptr8;
}

/* 00164928 0000001c */ static F26Dot6 fnt_FastIRound(/* a0 4 */ fnt_GlobalGraphicStateType *globalGS, /* a1 5 */ F26Dot6 value) {}
/* 00164948 00000068 */ static F26Dot6 fnt_IRound(/* v1 3 */ fnt_GlobalGraphicStateType *globalGS, /* a1 5 */ F26Dot6 value) {}
/* 001649b0 00000024 */ static F26Dot6 fnt_FixRound(/* v0 2 */ fnt_GlobalGraphicStateType *globalGS, /* a1 5 */ F26Dot6 value) {}

/* 001649d8 00000448 */ void SetScale_T2KTTClass(/* s3 19 */ T2KTTClass *t, /* v1 3 */ int xPixelsPerEm, /* a2 6 */ int yPixelsPerEm) {
	/* s1 17 */ fnt_GlobalGraphicStateType *g;
	/* s0 16 */ int i;
	/* s5 21 */ int n;
	/* s6 22 */ int16 nScale;
	/* fp 30 */ int16 dShift;
	/* s4 20 */ int32 dScale;
	/* s7 23 */ int32 dScaleDiv2;
	/* 0x20(sp) */ F16Dot16 fixedScale;
	/* 0x0(sp) */ T2KScaleInfo scale;
	/* v0 2 */ int32 tmp32;
	/* a1 5 */ int32 tmp32;
}

/* 00164e20 00000318 */ void GridOutline_T2KTTClass(/* s3 19 */ T2KTTClass *t, /* s4 20 */ GlyphClass *glyph) {
	/* s1 17 */ int i;
	/* s5 21 */ int n;
	/* a3 7 */ F26Dot6 *ptr32;
	/* s2 18 */ F16Dot16 multiplier;
	/* a0 4 */ F26Dot6 *oz;
	/* v0 2 */ F26Dot6 *z;
	/* v0 2 */ int16 *ptr16;
	/* v0 2 */ int16 *ptr16;
	/* t1 9 */ F26Dot6 *x;
	/* a2 6 */ int dx;
	/* t1 9 */ F26Dot6 *y;
	/* a2 6 */ int dy;
}

/* 00165138 000000dc */ void Delete_T2KTTClass(/* s1 17 */ T2KTTClass *t) {
	/* s3 19 */ int i;
	/* s0 16 */ fnt_GlobalGraphicStateType *g;
}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\autogrid.c
// *****************************************************************************

/* 00165218 00000034 */ static int ag_IsHinthandle(/* a0 4 */ ag_HintHandleType hintHandle) {}

/* 00165250 00000050 */ static int ag_T2K_Height_Shift(/* a0 4 */ ag_DataType *hData, /* a1 5 */ int yMultiplier, /* a2 6 */ short int UPEM, /* a3 7 */ int index, /* t0 8 */ short int *ooy, /* t1 9 */ int *oy, /* t2 10 */ int *y) {
	/* v0 2 */ int tmpY;
}

/* 001652a0 0000062c */ static int ag_SetUpCvt(/* s0 16 */ ag_DataType *hData, /* s7 23 */ int scale, /* 0x0(sp) */ char *xWeightIsOne) {
	/* s2 18 */ int i;
	/* a2 6 */ int j;
	/* a0 4 */ int sign;
	/* a3 7 */ int tmp;
	/* t8 24 */ int xDelta;
	/* 0x4(sp) */ int xMultiplier;
	/* s4 20 */ int yMultiplier;
	/* s1 17 */ short int UPEM;
	/* fp 30 */ int *cvt;
	/* v0 2 */ int tmp2;
}

/* 001658d0 000000fc */ static void ag_T2KFastYAGScale(/* a0 4 */ ag_DataType *hData, /* a1 5 */ ag_ElementType *elem) {
	/* v1 3 */ short int *ooy;
	/* a1 5 */ int *y;
	/* s1 17 */ int i;
	/* s0 16 */ int zone;
	/* v0 2 */ int limit;
	/* a1 5 */ int yin;
	/* s6 22 */ int y1;
	/* s5 21 */ int y2;
	/* s4 20 */ int y3;
	/* s7 23 */ int y4;
	/* 0x0(sp) */ int *add;
	/* fp 30 */ F16Dot16 *fmul;
}

/* 001659d0 000000b8 */ static int ag_ProcessOutline(/* s0 16 */ ag_DataType *hData, /* s1 17 */ ag_ElementType *elem, /* a2 6 */ short int isFigure, /* a3 7 */ short int curveType, /* s3 19 */ short int cmd, /* t1 9 */ short int **xDist, /* t2 10 */ int *xDistCount, /* t3 11 */ short int **yDist, /* 0x50(sp) */ int *yDistCount) {
	/* s2 18 */ int returnCode;
}

/* 00165a88 00000138 */ int ag_SetHintInfo(/* s2 18 */ ag_HintHandleType hintHandle, /* s1 17 */ ag_GlobalDataType *gDataIn, /* s0 16 */ ag_FontCategory fontType) {
	/* v0 2 */ int i;
}

/* 00165bc0 000000c4 */ int ag_AutoGridOutline(/* s0 16 */ ag_HintHandleType hintHandle, /* s1 17 */ ag_ElementType *elem, /* a2 6 */ short int cmd, /* a3 7 */ short int isFigure, /* t0 8 */ short int curveType, /* t1 9 */ short int grayScale, /* t2 10 */ short int numSBPointsIn) {
	/* v0 2 */ int errorCode;
}

/* 00165c88 000000cc */ int ag_HintInit(/* s1 17 */ tsiMemObject *mem, /* s0 16 */ int maxPointCount, /* a2 6 */ short int unitsPerEm, /* s3 19 */ ag_HintHandleType *hintHandle) {
	/* t2 10 */ ag_DataType *hData;
}

/* 00165d58 00000048 */ int ag_HintEnd(/* a0 4 */ ag_HintHandleType hintHandle) {
	/* s0 16 */ ag_DataType *hData;
}

/* 00165da0 00000070 */ int ag_SetScale(/* s0 16 */ ag_HintHandleType hintHandle, /* s1 17 */ int xPixelsPerEm, /* s2 18 */ int yPixelsPerEm, /* s3 19 */ char *xWeightIsOne) {}
/* 00165e10 00000008 */ static int CheckMaxPoints(/* a0 4 */ ag_HintHandleType hintHandle, /* a1 5 */ ag_ElementType *elem) {}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\util.c
// *****************************************************************************

/* 00165e18 00000054 */ void util_SortShortArray(/* a0 4 */ short int *a, /* a1 5 */ int n) {
	/* t1 9 */ short int change;
	/* a3 7 */ int i;
}

/* 00165e70 00000068 */ F16Dot16 util_FixMul(/* a0 4 */ F16Dot16 mA, /* v1 3 */ F16Dot16 mB) {
	/* v0 2 */ uint32 d1;
	/* v1 3 */ uint32 d2;
	/* a1 5 */ uint32 d3;
	/* v0 2 */ F16Dot16 result;
	/* a3 7 */ int sign;
}

/* 00165ed8 00000070 */ F16Dot16 util_FixMul26Dot6(/* v0 2 */ F16Dot16 mA, /* a1 5 */ F16Dot16 mB) {
	/* v0 2 */ uint32 d1;
	/* v1 3 */ uint32 d2;
	/* a0 4 */ uint32 d3;
	/* v0 2 */ F16Dot16 result;
	/* t0 8 */ int sign;
}

/* 00165f48 00000068 */ int util_ShortFracMul(/* a2 6 */ int mA, /* v0 2 */ short int mB) {
	/* a0 4 */ uint16 mB_Lo;
	/* v0 2 */ uint32 d2;
	/* v1 3 */ uint32 d3;
	/* v0 2 */ F16Dot16 result;
	/* t0 8 */ int sign;
}

/* 00165fb0 00000094 */ F16Dot16 util_FixDiv(/* a0 4 */ F16Dot16 mA, /* a1 5 */ F16Dot16 mB) {
	/* a3 7 */ int sign;
	/* v1 3 */ uint32 high16;
	/* a0 4 */ uint32 low16;
	/* v0 2 */ F16Dot16 Q;
}

/* 00166048 000000d4 */ F16Dot16 util_FixSin(/* a0 4 */ F16Dot16 in) {
	/* a0 4 */ F16Dot16 node1pow1;
	/* v1 3 */ F16Dot16 node1pow2;
	/* a0 4 */ F16Dot16 node1pow3;
	/* a1 5 */ F16Dot16 node1pow4;
	/* v1 3 */ F16Dot16 node1pow5;
	/* v0 2 */ F16Dot16 out;
}

/* 00166120 00000160 */ F16Dot16 util_EuclidianDistance(/* s2 18 */ F16Dot16 A, /* s3 19 */ F16Dot16 B) {
	/* s1 17 */ F16Dot16 root;
}

/* 00166280 00000128 */ void util_ComputeIntersection(/* a0 4 */ short int line1_pt1_x, /* a1 5 */ short int line1_pt1_y, /* a2 6 */ short int line1_pt2_x, /* t4 12 */ short int line1_pt2_y, /* s1 17 */ short int line2_pt1_x, /* s2 18 */ short int line2_pt1_y, /* t2 10 */ short int line2_pt2_x, /* t3 11 */ short int line2_pt2_y, /* 0x80(sp) */ short int *x, /* 0x88(sp) */ short int *y) {
	/* s5 21 */ short int *x;
	/* s6 22 */ short int *y;
	/* a0 4 */ int dx1;
	/* a1 5 */ int dy1;
	/* s3 19 */ int dx2;
	/* s4 20 */ int dy2;
	/* v1 3 */ int Num;
	/* a1 5 */ int Denom;
	/* s0 16 */ int t;
}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\fnt.c
// *****************************************************************************

/* 001663a8 00000008 */ void fnt_Panic(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ int error) {}
/* 001663b0 0000001c */ static void fnt_IllegalInstruction(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 001663d0 0000008c */ static void fnt_Normalize(/* s2 18 */ F26Dot6 x, /* s1 17 */ F26Dot6 y, /* s3 19 */ VECTOR *v) {
	/* s0 16 */ F16Dot16 length;
}

/* 00166460 0000004c */ F26Dot6 fnt_RoundToDoubleGrid(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 xin, /* a2 6 */ F26Dot6 engine) {
	/* a0 4 */ F26Dot6 x;
}

/* 001664b0 00000044 */ F26Dot6 fnt_RoundDownToGrid(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 xin, /* a2 6 */ F26Dot6 engine) {
	/* a0 4 */ F26Dot6 x;
}

/* 001664f8 0000004c */ F26Dot6 fnt_RoundUpToGrid(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 xin, /* a2 6 */ F26Dot6 engine) {
	/* a0 4 */ F26Dot6 x;
}

/* 00166548 0000004c */ F26Dot6 fnt_RoundToGrid(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 xin, /* a2 6 */ F26Dot6 engine) {
	/* a0 4 */ F26Dot6 x;
}

/* 00166598 00000054 */ F26Dot6 fnt_RoundToHalfGrid(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 xin, /* a2 6 */ F26Dot6 engine) {
	/* a0 4 */ F26Dot6 x;
}

/* 001665f0 00000008 */ F26Dot6 fnt_RoundOffSLOW(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 xin, /* a2 6 */ F26Dot6 engine) {}

/* 001665f8 0000008c */ F26Dot6 fnt_SuperRound(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 xin, /* a2 6 */ F26Dot6 engine) {
	/* a3 7 */ F26Dot6 x;
	/* t0 8 */ fnt_ParameterBlock *pb;
}

/* 00166688 000000dc */ F26Dot6 fnt_Super45Round(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* s1 17 */ F26Dot6 xin, /* a2 6 */ F26Dot6 engine) {
	/* a3 7 */ F26Dot6 x;
	/* s0 16 */ fnt_ParameterBlock *pb;
}

/* 00166768 00000178 */ static void fnt_MovePoint(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* s0 16 */ fnt_ElementType *element, /* s1 17 */ ArrayIndex point, /* s3 19 */ F26Dot6 delta) {
	/* s2 18 */ ShortFrac pfProj;
	/* a1 5 */ ShortFrac fx;
	/* s4 20 */ ShortFrac fy;
}

/* 001668e0 00000030 */ static void fnt_XMovePoint(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ fnt_ElementType *element, /* a2 6 */ ArrayIndex point, /* a3 7 */ F26Dot6 delta) {}
/* 00166910 00000030 */ static void fnt_YMovePoint(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ fnt_ElementType *element, /* a2 6 */ ArrayIndex point, /* a3 7 */ F26Dot6 delta) {}
/* 00166940 00000054 */ static F26Dot6 fnt_Project(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 x, /* s2 18 */ F26Dot6 y) {}
/* 00166998 00000054 */ static F26Dot6 fnt_OldProject(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 x, /* s2 18 */ F26Dot6 y) {}
/* 001669f0 00000008 */ static F26Dot6 fnt_XProject(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 x, /* a2 6 */ F26Dot6 y) {}
/* 001669f8 00000008 */ static F26Dot6 fnt_YProject(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 x, /* a2 6 */ F26Dot6 y) {}

/* 00166a00 00000080 */ static Fixed fnt_GetCVTScale(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s0 16 */ Fixed scale;
	/* s1 17 */ fnt_GlobalGraphicStateType *globalGS;
}

/* 00166a80 00000034 */ static F26Dot6 fnt_GetCVTEntryFast(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ ArrayIndex n) {}

/* 00166ab8 00000058 */ static F26Dot6 fnt_GetCVTEntrySlow(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ ArrayIndex n) {
	/* s0 16 */ F26Dot6 cvtValue;
}

/* 00166b10 0000000c */ static F26Dot6 fnt_GetSingleWidthFast(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 00166b20 00000034 */ static F26Dot6 fnt_GetSingleWidthSlow(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00166b58 00000090 */ static void fnt_ChangeCvt(/* s1 17 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ fnt_ElementType *elem, /* s2 18 */ ArrayIndex number, /* s0 16 */ F26Dot6 delta) {
	/* a1 5 */ Fixed scale;
}

/* 00166be8 000001a4 */ static void fnt_InnerTraceExecute(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ uint8 *ptr, /* s1 17 */ uint8 *eptr) {
	/* s2 18 */ FntFunc *function;
	/* 0x0(sp) */ uint8 *oldInsPtr;
	/* s3 19 */ fnt_ParameterBlock *pb;
}

/* 00166d90 00000094 */ static void fnt_InnerExecute(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ uint8 *ptr, /* s1 17 */ uint8 *eptr) {
	/* s2 18 */ FntFunc *function;
	/* s3 19 */ uint8 *oldInsPtr;
}

/* 00166e28 000001f8 */ int fnt_Execute(/* a0 4 */ fnt_ElementType *elements, /* s4 20 */ uint8 *ptr, /* s5 21 */ uint8 *eptr, /* s1 17 */ fnt_GlobalGraphicStateType *globalGS, /* s3 19 */ fs_FuncType TraceFunc) {
	/* 0x0(sp) */ fnt_LocalGraphicStateType thisLocalGS;
	/* s0 16 */ fnt_LocalGraphicStateType *pLocalGS;
}

/* 00167020 00000038 */ static void fnt_Check_PF_Proj(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00167058 00000064 */ static void fnt_ComputeAndCheck_PF_Proj(/* a2 6 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v1 3 */ ShortFrac pfProj;
}

/* 001670c0 00000038 */ static void fnt_SVTCA_0(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 001670f8 00000038 */ static void fnt_SVTCA_1(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 00167130 0000008c */ static void fnt_SPVTCA(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 001671c0 00000068 */ static void fnt_SFVTCA(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00167228 000000d4 */ static void fnt_SPVTL(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ ArrayIndex arg1;
	/* a1 5 */ ArrayIndex arg2;
}

/* 00167300 00000148 */ static void fnt_SDPVTL(/* s2 18 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s0 16 */ ArrayIndex arg1;
	/* s1 17 */ ArrayIndex arg2;
}

/* 00167448 000000c4 */ static void fnt_SFVTL(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ ArrayIndex arg1;
	/* a1 5 */ ArrayIndex arg2;
}

/* 00167510 00000064 */ static void fnt_WPV(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 00167578 00000054 */ static void fnt_WFV(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 001675d0 00000028 */ static void fnt_RPV(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 001675f8 00000028 */ static void fnt_RFV(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 00167620 00000028 */ static void fnt_SFVTPV(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00167648 00000358 */ static void fnt_ISECT(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s1 17 */ F26Dot6 N;
	/* a1 5 */ F26Dot6 D;
	/* s1 17 */ F16Dot16 t;
	/* s0 16 */ ArrayIndex arg1;
	/* a3 7 */ ArrayIndex arg2;
	/* s3 19 */ F26Dot6 Bx;
	/* s5 21 */ F26Dot6 By;
	/* fp 30 */ F26Dot6 Ax;
	/* 0x0(sp) */ F26Dot6 Ay;
	/* s2 18 */ F26Dot6 dBx;
	/* s4 20 */ F26Dot6 dBy;
	/* s6 22 */ F26Dot6 dAx;
	/* s7 23 */ F26Dot6 dAy;
	/* t4 12 */ fnt_ElementType *element;
	/* v0 2 */ F26Dot6 *stack;
	/* 0x4(sp) */ F26Dot6 *elementx;
	/* 0x8(sp) */ F26Dot6 *elementy;
}

/* 001679a0 0000001c */ static void fnt_LMD(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 001679c0 0000001c */ static void fnt_LWTCI(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 001679e0 0000001c */ static void fnt_LSWCI(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00167a00 00000050 */ static void fnt_LSW(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v1 3 */ fnt_GlobalGraphicStateType *globalGS;
	/* s0 16 */ fnt_ParameterBlock *pb;
}

/* 00167a50 00000068 */ static void fnt_SetLocalGraphicState(/* a1 5 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a2 6 */ int arg;
}

/* 00167ab8 00000084 */ static void fnt_SetElementPtr(/* t0 8 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ ArrayIndex arg;
	/* a1 5 */ fnt_ElementType *element;
}

/* 00167b40 0000004c */ static void fnt_SROUND(/* v1 3 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s0 16 */ fnt_ParameterBlock *pb;
}

/* 00167b90 0000004c */ static void fnt_S45ROUND(/* v1 3 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s0 16 */ fnt_ParameterBlock *pb;
}

/* 00167be0 000000c0 */ static void fnt_SetRoundState(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00167ca0 0000017c */ static void fnt_SetRoundValues(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ int arg1, /* a2 6 */ int normalRound) {
	/* a0 4 */ int tmp;
	/* a3 7 */ fnt_ParameterBlock *pb;
}

/* 00167e20 0000004c */ static void fnt_RAW(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ fnt_ElementType *elem;
	/* a3 7 */ F26Dot6 *ox;
	/* v0 2 */ ArrayIndex index;
}

/* 00167e70 00000018 */ static void fnt_DUP(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v1 3 */ F26Dot6 top;
}

/* 00167e88 00000010 */ static void fnt_CLEAR(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00167e98 00000030 */ static void fnt_SWAP(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a1 5 */ F26Dot6 arg2;
	/* a2 6 */ F26Dot6 arg1;
}

/* 00167ec8 00000024 */ static void fnt_DEPTH(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00167ef0 00000028 */ static void fnt_CINDEX(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ ArrayIndex arg1;
	/* a1 5 */ F26Dot6 tmp;
}

/* 00167f18 00000050 */ static void fnt_MINDEX(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a1 5 */ ArrayIndex arg1;
	/* a3 7 */ F26Dot6 tmp;
	/* v1 3 */ F26Dot6 *p;
	/* a2 6 */ F26Dot6 *stack;
}

/* 00167f68 0000002c */ static void fnt_ROTATE(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ F26Dot6 *stack;
	/* a0 4 */ F26Dot6 element1;
	/* v1 3 */ F26Dot6 element2;
}

/* 00167f98 000000d4 */ static void fnt_MDAP(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s1 17 */ F26Dot6 proj;
	/* s3 19 */ fnt_ElementType *ce0;
	/* s4 20 */ fnt_ParameterBlock *pb;
	/* s2 18 */ ArrayIndex ptNum;
}

/* 00168070 0000016c */ static void fnt_MIAP(/* s1 17 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s4 20 */ ArrayIndex ptNum;
	/* s3 19 */ F26Dot6 newProj;
	/* s0 16 */ F26Dot6 origProj;
	/* s2 18 */ fnt_ElementType *ce0;
	/* s6 22 */ fnt_ParameterBlock *pb;
	/* a1 5 */ F26Dot6 tmp;
}

/* 001681e0 00000450 */ static void fnt_IUP(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s0 16 */ ArrayIndex pt;
	/* t2 10 */ int16 *ooCoord;
	/* a1 5 */ LoopCount ctr;
	/* t1 9 */ F26Dot6 *coord;
	/* t7 15 */ F26Dot6 *oCoord;
	/* 0x0(sp) */ int mask;
	/* 0x4(sp) */ ArrayIndex ptOrg;
	/* s2 18 */ ArrayIndex end;
	/* s7 23 */ ArrayIndex ep_ctr;
	/* fp 30 */ ArrayIndex sp_ctr;
	/* t9 25 */ fnt_ElementType *pCE2;
	/* 0x8(sp) */ uint8 *pFlags;
	/* a2 6 */ ArrayIndex start;
	/* v0 2 */ ArrayIndex ptMin;
	/* a2 6 */ ArrayIndex ptMax;
	/* s6 22 */ F26Dot6 _min;
	/* t5 13 */ F26Dot6 o_min;
	/* t3 11 */ F26Dot6 oo_min;
	/* t4 12 */ F26Dot6 dmin;
	/* s3 19 */ F26Dot6 oo_delta;
	/* t6 14 */ F26Dot6 dmax;
	/* s4 20 */ F26Dot6 _delta;
	/* s5 21 */ F26Dot6 o_max;
	/* a3 7 */ F26Dot6 corr;
	/* v1 3 */ F26Dot6 oCoord_pt;
	/* a2 6 */ int32 tmp;
	/* a3 7 */ Fixed ratio;
	/* t0 8 */ int firstTime;
}

/* 00168630 00000168 */ static fnt_ElementType* fnt_SH_Common(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS, /* s4 20 */ F26Dot6 *dx, /* s5 21 */ F26Dot6 *dy, /* s6 22 */ ArrayIndex *point) {
	/* s1 17 */ F26Dot6 proj;
	/* s3 19 */ ArrayIndex pt;
	/* s2 18 */ fnt_ElementType *element;
}

/* 00168798 000000ac */ static void fnt_SHP_Common(/* t0 8 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ F26Dot6 dx, /* a2 6 */ F26Dot6 dy) {
	/* t1 9 */ fnt_ElementType *CE2;
	/* v1 3 */ LoopCount count;
	/* a3 7 */ ArrayIndex point;
}

/* 00168848 00000040 */ static void fnt_SHP(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* 0x0(sp) */ F26Dot6 dx;
	/* 0x4(sp) */ F26Dot6 dy;
	/* 0x8(sp) */ ArrayIndex point;
}

/* 00168888 00000138 */ static void fnt_SHC(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* t3 11 */ fnt_ElementType *element;
	/* t1 9 */ F26Dot6 dx;
	/* t0 8 */ F26Dot6 dy;
	/* a2 6 */ ArrayIndex contour;
	/* t2 10 */ ArrayIndex point;
	/* a3 7 */ fnt_ElementType *CE2;
	/* 0x0(sp) */ F26Dot6 x;
	/* 0x4(sp) */ F26Dot6 y;
	/* 0x8(sp) */ ArrayIndex pt;
	/* a1 5 */ ArrayIndex currPt;
	/* v0 2 */ LoopCount count;
}

/* 001689c0 0000023c */ static void fnt_SHE(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* t2 10 */ F26Dot6 dx;
	/* t4 12 */ F26Dot6 dy;
	/* t0 8 */ ArrayIndex firstPoint;
	/* 0x8(sp) */ ArrayIndex origPoint;
	/* t3 11 */ ArrayIndex lastPoint;
	/* t5 13 */ ArrayIndex arg1;
	/* 0x0(sp) */ F26Dot6 x;
	/* 0x4(sp) */ F26Dot6 y;
	/* t1 9 */ int8 mask;
	/* a0 4 */ F26Dot6 *x;
	/* v0 2 */ LoopCount count;
	/* a0 4 */ F26Dot6 *y;
	/* a1 5 */ uint8 *f;
	/* v0 2 */ LoopCount count;
}

/* 00168c00 0000008c */ static void fnt_SHPIX(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s1 17 */ F26Dot6 proj;
	/* s2 18 */ F26Dot6 dx;
	/* s3 19 */ F26Dot6 dy;
}

/* 00168c90 00000210 */ static void fnt_IP(/* s3 19 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ F26Dot6 oldRange;
	/* s4 20 */ F26Dot6 currentRange;
	/* v0 2 */ ArrayIndex RP1;
	/* s0 16 */ ArrayIndex pt2;
	/* s5 21 */ fnt_ElementType *CE0;
	/* s1 17 */ fnt_ElementType *CE1;
	/* s6 22 */ fnt_ElementType *CE2;
	/* v1 3 */ LoopCount loop;
	/* fp 30 */ Fixed multiplier;
	/* s2 18 */ ArrayIndex arg1;
	/* s1 17 */ F26Dot6 tmp;
}

/* 00168ea0 0000018c */ static void fnt_MSIRP(/* s2 18 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s4 20 */ fnt_ElementType *CE0;
	/* s3 19 */ fnt_ElementType *CE1;
	/* s7 23 */ ArrayIndex Pt0;
	/* s6 22 */ F26Dot6 dist;
	/* s5 21 */ ArrayIndex pt2;
}

/* 00169030 000000e0 */ static void fnt_ALIGNRP(/* s1 17 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s2 18 */ fnt_ElementType *ce1;
	/* s3 19 */ F26Dot6 pt0x;
	/* s4 20 */ F26Dot6 pt0y;
	/* s0 16 */ ArrayIndex ptNum;
}

/* 00169110 000000e4 */ static void fnt_ALIGNPTS(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s3 19 */ ArrayIndex pt1;
	/* s4 20 */ ArrayIndex pt2;
	/* s1 17 */ F26Dot6 move1;
	/* s2 18 */ F26Dot6 dist;
}

/* 001691f8 0000001c */ static void fnt_SANGW(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 00169218 00000048 */ static Fract fnt_QuickDist(/* t0 8 */ Fract dx, /* v1 3 */ Fract dy) {}

/* 00169260 00000058 */ static void fnt_FLIPPT(/* a3 7 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* t0 8 */ uint8 *onCurve;
	/* a1 5 */ F26Dot6 *stack;
	/* v1 3 */ LoopCount count;
	/* v1 3 */ ArrayIndex point;
}

/* 001692b8 0000006c */ static void fnt_FLIPRGON(/* a3 7 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a0 4 */ ArrayIndex lo;
	/* v1 3 */ ArrayIndex hi;
	/* v1 3 */ LoopCount count;
	/* a1 5 */ uint8 *onCurve;
	/* a2 6 */ F26Dot6 *stack;
}

/* 00169328 0000006c */ static void fnt_FLIPRGOFF(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v1 3 */ ArrayIndex lo;
	/* v0 2 */ ArrayIndex hi;
	/* v0 2 */ LoopCount count;
	/* a3 7 */ uint8 *onCurve;
}

/* 00169398 00000030 */ static void fnt_SCANCTRL(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a1 5 */ fnt_GlobalGraphicStateType *globalGS;
	/* a1 5 */ fnt_ParameterBlock *pb;
}

/* 001693c8 00000084 */ static void fnt_SCANTYPE(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v1 3 */ fnt_GlobalGraphicStateType *globalGS;
	/* v1 3 */ int value;
	/* a2 6 */ int32 *scanPtr;
}

/* 00169450 00000068 */ static void fnt_INSTCTRL(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a2 6 */ fnt_GlobalGraphicStateType *globalGS;
	/* a3 7 */ int selector;
	/* a0 4 */ int32 value;
}

/* 001694b8 00000360 */ static void fnt_AA(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* 0x10(sp) */ fnt_GlobalGraphicStateType *globalGS;
	/* 0x14(sp) */ ArrayIndex ptNum;
	/* 0x18(sp) */ ArrayIndex bestAngle;
	/* s5 21 */ F26Dot6 dx;
	/* s4 20 */ F26Dot6 dy;
	/* a3 7 */ F26Dot6 tmp32;
	/* 0x1c(sp) */ Fract pvx;
	/* 0x20(sp) */ Fract pvy;
	/* 0x24(sp) */ Fract pfProj;
	/* 0x28(sp) */ Fract tpvx;
	/* 0x2c(sp) */ Fract tpvy;
	/* s1 17 */ Fract *anglePoint;
	/* s2 18 */ int16 distance;
	/* s3 19 */ int16 *angleDistance;
	/* 0x30(sp) */ int32 minPenalty;
	/* fp 30 */ LoopCount i;
	/* 0x34(sp) */ int yFlip;
	/* 0x38(sp) */ int xFlip;
	/* 0x3c(sp) */ int xySwap;
	/* 0x0(sp) */ VECTOR v;
}

/* 00169818 000000c0 */ static void fnt_PushSomeStuff(/* t1 9 */ fnt_LocalGraphicStateType_t *pLocalGS, /* v0 2 */ LoopCount count, /* a2 6 */ int pushBytes) {
	/* t0 8 */ F26Dot6 *stack;
	/* a3 7 */ uint8 *instr;
}

/* 001698d8 0000002c */ static void fnt_PUSHB(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 00169908 00000030 */ static void fnt_NPUSHB(/* v0 2 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 00169938 0000002c */ static void fnt_PUSHW(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 00169968 00000030 */ static void fnt_NPUSHW(/* v0 2 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00169998 00000060 */ static void fnt_WS(/* a2 6 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a3 7 */ F26Dot6 storage;
	/* a1 5 */ ArrayIndex storeIndex;
}

/* 001699f8 00000064 */ static void fnt_RS(/* a3 7 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a2 6 */ ArrayIndex storeIndex;
}

/* 00169a60 00000090 */ static void fnt_WCVT(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s1 17 */ ArrayIndex cvtIndex;
	/* a3 7 */ F26Dot6 cvtValue;
	/* a2 6 */ F16Dot16 cvtScale;
}

/* 00169af0 00000064 */ static void fnt_WCVTFOD(/* v0 2 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s0 16 */ ArrayIndex cvtIndex;
	/* a1 5 */ F26Dot6 cvtValue;
	/* s1 17 */ fnt_GlobalGraphicStateType *globalGS;
}

/* 00169b58 00000048 */ static void fnt_RCVT(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00169ba0 00000098 */ static void fnt_RC(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ ArrayIndex pt;
	/* a2 6 */ fnt_ElementType *element;
	/* v1 3 */ F26Dot6 proj;
}

/* 00169c38 000000d4 */ static void fnt_WC(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s2 18 */ F26Dot6 coord;
	/* s1 17 */ ArrayIndex pt;
	/* s3 19 */ fnt_ElementType *element;
}

/* 00169d10 00000120 */ static void fnt_MD(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a1 5 */ ArrayIndex pt1;
	/* v1 3 */ ArrayIndex pt2;
	/* v0 2 */ F26Dot6 proj;
	/* s1 17 */ F26Dot6 *stack;
	/* s2 18 */ fnt_GlobalGraphicStateType *globalGS;
}

/* 00169e30 00000068 */ static void fnt_MPPEM(/* s1 17 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s0 16 */ uint16 ppem;
	/* v0 2 */ fnt_GlobalGraphicStateType *globalGS;
}

/* 00169e98 0000001c */ static void fnt_MPS(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00169eb8 00000054 */ static void fnt_GETINFO(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* t0 8 */ fnt_GlobalGraphicStateType *globalGS;
	/* a2 6 */ int selector;
	/* a1 5 */ int info;
}

/* 00169f10 00000010 */ static void fnt_FLIPON(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 00169f20 0000000c */ static void fnt_FLIPOFF(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 00169f30 00000010 */ static void fnt_DEBUG(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 00169f40 00000170 */ static void fnt_BinaryOperand(/* s1 17 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s0 16 */ F26Dot6 *stack;
	/* a2 6 */ F26Dot6 arg2;
	/* a1 5 */ F26Dot6 arg1;
}

/* 0016a0b0 000000dc */ static void fnt_UnaryOperand(/* s1 17 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s0 16 */ F26Dot6 *stack;
	/* a1 5 */ F26Dot6 arg;
	/* s2 18 */ int opCode;
}

/* 0016a190 00000078 */ static void fnt_SkipPushCrap(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a1 5 */ int opCode;
	/* v1 3 */ uint8 *instr;
	/* v0 2 */ ArrayIndex count;
}

/* 0016a208 000000d0 */ static void fnt_IF(/* s1 17 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s0 16 */ int level;
}

/* 0016a2d8 00000088 */ static void fnt_ELSE(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s1 17 */ int level;
}

/* 0016a360 00000008 */ static void fnt_EIF(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 0016a368 00000024 */ static void fnt_JMPR(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ ArrayIndex offset;
}

/* 0016a390 00000030 */ static void fnt_JROT(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ ArrayIndex offset;
	/* a1 5 */ F26Dot6 *stack;
}

/* 0016a3c0 00000030 */ static void fnt_JROF(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ ArrayIndex offset;
	/* a1 5 */ F26Dot6 *stack;
}

/* 0016a3f0 00000050 */ static void fnt_ROUND(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 0016a440 0000000c */ static void fnt_NROUND(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a1 5 */ F26Dot6 arg1;
}

/* 0016a450 00000094 */ static F26Dot6 fnt_CheckSingleWidth(/* v1 3 */ fnt_LocalGraphicStateType_t *pLocalGS, /* s0 16 */ F26Dot6 value) {
	/* a2 6 */ F26Dot6 delta;
	/* a3 7 */ F26Dot6 scaledSW;
	/* s1 17 */ fnt_ParameterBlock *pb;
}

/* 0016a4e8 00000238 */ static void fnt_MDRP(/* s1 17 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s6 22 */ ArrayIndex pt1;
	/* fp 30 */ ArrayIndex pt0;
	/* s0 16 */ F26Dot6 tmp;
	/* s5 21 */ F26Dot6 tmpC;
	/* s2 18 */ fnt_ElementType *element;
	/* s0 16 */ fnt_GlobalGraphicStateType *globalGS;
	/* s7 23 */ fnt_ParameterBlock *pb;
	/* v1 3 */ F26Dot6 tmpB;
}

/* 0016a720 0000030c */ static void fnt_MIRP(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* fp 30 */ ArrayIndex ptNum;
	/* a1 5 */ ArrayIndex Pt0;
	/* s1 17 */ F26Dot6 tmp;
	/* a1 5 */ F26Dot6 tmpB;
	/* s6 22 */ F26Dot6 tmpC;
	/* s2 18 */ F26Dot6 tmpProj;
	/* s7 23 */ fnt_ParameterBlock *pb;
	/* s4 20 */ fnt_ElementType *pCE0;
	/* s5 21 */ fnt_ElementType *pCE1;
	/* 0x0(sp) */ unsigned int opCode;
}

/* 0016aa30 00000060 */ static void fnt_CALL(/* v1 3 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v1 3 */ fnt_funcDef *funcDef;
	/* a1 5 */ uint8 *ins;
	/* a3 7 */ fnt_GlobalGraphicStateType *globalGS;
	/* v0 2 */ ArrayIndex arg;
}

/* 0016aa90 000000cc */ static void fnt_FDEF(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s2 18 */ fnt_funcDef *funcDef;
	/* v1 3 */ uint8 *program;
	/* s1 17 */ uint8 *funcStart;
	/* a0 4 */ fnt_GlobalGraphicStateType *globalGS;
	/* v1 3 */ ArrayIndex arg;
}

/* 0016ab60 000000d8 */ static void fnt_LOOPCALL(/* s1 17 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s4 20 */ uint8 *start;
	/* s3 19 */ uint8 *stop;
	/* s5 21 */ InterpreterFunc Interpreter;
	/* v1 3 */ fnt_funcDef *funcDef;
	/* v0 2 */ ArrayIndex arg;
	/* v0 2 */ LoopCount loop;
	/* v1 3 */ uint8 *ins;
}

/* 0016ac38 00000058 */ static fnt_instrDef* fnt_FindIDef(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS, /* a1 5 */ uint8 opCode) {
	/* v1 3 */ fnt_GlobalGraphicStateType *globalGS;
	/* v0 2 */ LoopCount count;
	/* a0 4 */ fnt_instrDef *instrDef;
}

/* 0016ac90 00000070 */ static void fnt_IDefPatch(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a1 5 */ fnt_instrDef *instrDef;
	/* a1 5 */ uint8 *program;
}

/* 0016ad00 000000f4 */ static void fnt_IDEF(/* s0 16 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* s2 18 */ uint8 opCode;
	/* s1 17 */ fnt_instrDef *instrDef;
	/* a2 6 */ ArrayIndex pgmIndex;
	/* a3 7 */ uint8 *program;
	/* s3 19 */ uint8 *instrStart;
}

/* 0016adf8 00000054 */ static void fnt_UTP(/* a2 6 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* a3 7 */ ArrayIndex point;
	/* a1 5 */ uint8 *f;
}

/* 0016ae50 0000001c */ static void fnt_SDB(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}
/* 0016ae70 0000001c */ static void fnt_SDS(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {}

/* 0016ae90 00000188 */ static void fnt_DeltaEngine(/* s2 18 */ fnt_LocalGraphicStateType_t *pLocalGS, /* s5 21 */ FntMoveFunc doIt, /* s1 17 */ int16 base, /* s6 22 */ int16 shift) {
	/* a3 7 */ int32 tmp;
	/* s1 17 */ int32 fakePixelsPerEm;
	/* v0 2 */ int32 ppem;
	/* s0 16 */ int32 aim;
	/* s3 19 */ int32 high;
	/* s0 16 */ int32 tmp32;
}

/* 0016b018 0000002c */ static void fnt_DELTAP1(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ fnt_ParameterBlock *pb;
}

/* 0016b048 00000038 */ static void fnt_DELTAP2(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ fnt_ParameterBlock *pb;
}

/* 0016b080 00000038 */ static void fnt_DELTAP3(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ fnt_ParameterBlock *pb;
}

/* 0016b0b8 00000030 */ static void fnt_DELTAC1(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ fnt_ParameterBlock *pb;
}

/* 0016b0e8 0000003c */ static void fnt_DELTAC2(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ fnt_ParameterBlock *pb;
}

/* 0016b128 0000003c */ static void fnt_DELTAC3(/* a0 4 */ fnt_LocalGraphicStateType_t *pLocalGS) {
	/* v0 2 */ fnt_ParameterBlock *pb;
}

/* 0016b168 00000890 */ static void fnt_DefaultJumpTable(/* fp 30 */ FntFunc *function) {}
/* 0016b9f8 00000340 */ void fnt_Init(/* s0 16 */ fnt_GlobalGraphicStateType *globalGS) {}

// *****************************************************************************
// FILE -- F:\eapackages\FontFusion\2.3.0-1\core\t2kstrk1.c
// *****************************************************************************

/* 0016bd38 0000007c */ static PathClass* New_PathClass(/* s1 17 */ tsiMemObject *mem) {
	/* s0 16 */ PathClass *t;
}

/* 0016bdb8 00000044 */ static void Delete_PathClass(/* s0 16 */ PathClass *t) {}

/* 0016be00 000000ac */ static void AddLeftPoint(/* s0 16 */ PathClass *t, /* s3 19 */ short int x, /* s4 20 */ short int y, /* s2 18 */ short int onOff) {
	/* s1 17 */ int count;
	/* v0 2 */ short int *xyLeft;
}

/* 0016beb0 000000e8 */ static void Add2LeftPoints(/* s0 16 */ PathClass *t, /* s3 19 */ short int x1, /* s4 20 */ short int y1, /* s5 21 */ short int onOff1, /* s6 22 */ short int x2, /* s7 23 */ short int y2, /* s2 18 */ short int onOff2) {
	/* s1 17 */ int count;
	/* v0 2 */ short int *xyLeft;
}

/* 0016bf98 000000ac */ static void AddRightPoint(/* s0 16 */ PathClass *t, /* s3 19 */ short int x, /* s4 20 */ short int y, /* s2 18 */ short int onOff) {
	/* s1 17 */ int count;
	/* v0 2 */ short int *xyRight;
}

/* 0016c048 000000b0 */ static int Distance(/* a3 7 */ int dx, /* t0 8 */ int dy) {
	/* a2 6 */ int root;
	/* v1 3 */ int square;
}

/* 0016c0f8 00000048 */ static int ApproxDistance(/* v1 3 */ int dx, /* t0 8 */ int dy) {}

/* 0016c140 0000019c */ static int IsIntersection(/* a0 4 */ short int line1_pt1_x, /* a1 5 */ short int line1_pt1_y, /* t4 12 */ short int line1_pt2_x, /* t5 13 */ short int line1_pt2_y, /* s1 17 */ short int line2_pt1_x, /* s3 19 */ short int line2_pt1_y, /* t2 10 */ short int line2_pt2_x, /* t3 11 */ short int line2_pt2_y, /* 0xb0(sp) */ short int *x, /* 0xb8(sp) */ short int *y) {
	/* a0 4 */ int dx1;
	/* a1 5 */ int dy1;
	/* s2 18 */ int dx2;
	/* s6 22 */ int dy2;
	/* a0 4 */ int Num;
	/* a1 5 */ int Denom;
	/* s0 16 */ int t1;
	/* 0x0(sp) */ int result;
}

/* 0016c2e0 00000968 */ static void lineJoin(/* 0x20(sp) */ PathClass *t, /* 0x24(sp) */ int joinType, /* 0x28(sp) */ int x1, /* 0x2c(sp) */ int y1, /* 0x30(sp) */ int x2, /* 0x34(sp) */ int y2, /* s6 22 */ int onOff2, /* s5 21 */ int pointNum2, /* 0xf0(sp) */ int x3, /* 0xf8(sp) */ int y3, /* 0x100(sp) */ int r) {
	/* s3 19 */ int rot1Dx;
	/* s4 20 */ int rot1Dy;
	/* fp 30 */ int rot2Dx;
	/* 0x38(sp) */ int rot2Dy;
	/* v0 2 */ F16Dot16 length;
	/* s0 16 */ F16Dot16 mul;
	/* 0x10(sp) */ short int xLeft;
	/* 0x12(sp) */ short int yLeft;
	/* 0x14(sp) */ short int xRight;
	/* 0x16(sp) */ short int yRight;
	/* s5 21 */ short int xOutside;
	/* s2 18 */ short int yOutside;
	/* s1 17 */ int area;
	/* 0x3c(sp) */ int miterLimit;
	/* 0x40(sp) */ PF_ADD_POINT AddOutsidePoint;
	/* 0x44(sp) */ int iLeft;
	/* v0 2 */ int iRight;
	/* a0 4 */ int posArea;
	/* s3 19 */ int maxArea;
	/* s4 20 */ int direction;
	/* s3 19 */ int d1;
	/* 0x18(sp) */ short int p1x;
	/* 0x1a(sp) */ short int p1y;
	/* 0x1c(sp) */ short int p2x;
	/* 0x1e(sp) */ short int p2y;
	/* s5 21 */ short int dx;
	/* s1 17 */ short int dy;
}

/* 0016cc48 00000228 */ static void EndCap(/* fp 30 */ PathClass *t, /* s5 21 */ int capType, /* s6 22 */ int x, /* s7 23 */ int y, /* s1 17 */ int dx, /* s3 19 */ int dy, /* s2 18 */ int r) {
	/* s4 20 */ int xr;
	/* s2 18 */ int yr;
	/* s0 16 */ int rot1Dx;
	/* v0 2 */ F16Dot16 length;
	/* s0 16 */ F16Dot16 mul;
}

/* 0016ce70 000000f8 */ static void glyph_EnsureSpaceForNPoints(/* s0 16 */ GlyphClass *glyph, /* a1 5 */ int N) {
	/* t3 11 */ int limit;
	/* a2 6 */ int i;
	/* a0 4 */ short int *oox;
	/* v1 3 */ short int *ooy;
	/* t4 12 */ uint8 *onCurve;
	/* s3 19 */ short int *ooxOld;
	/* s2 18 */ short int *ooyOld;
	/* s4 20 */ uint8 *onCurveOld;
	/* s1 17 */ F26Dot6 *memBase;
}

/* 0016cf68 00000054 */ static int myDist(/* a0 4 */ PathClass *path, /* a1 5 */ int leftIndex, /* a2 6 */ int rightIndex) {
	/* v0 2 */ int dx;
	/* v1 3 */ int dy;
}

/* 0016cfc0 00000144 */ static int IsOverExtended(/* s1 17 */ PathClass *path) {
	/* s3 19 */ int dist_0;
	/* s6 22 */ int dist_180;
	/* s2 18 */ int leftIndex;
	/* s0 16 */ int rightIndex;
	/* s4 20 */ int rightMidIndex;
	/* s7 23 */ int leftCountDiv2;
}

/* 0016d108 00000334 */ static void glyph_MergePathIntoGlyph(/* s2 18 */ GlyphClass *glyph, /* s1 17 */ PathClass *path, /* s7 23 */ int open) {
	/* t0 8 */ int i;
	/* t3 11 */ int limit;
	/* s0 16 */ int point;
	/* s6 22 */ short int *oox;
	/* s4 20 */ short int *ooy;
	/* s3 19 */ uint8 *onCurve;
	/* s5 21 */ int ctr;
}

/* 0016d440 00000324 */ static void glyph_StrokeCtr(/* s3 19 */ GlyphClass *glyph, /* s0 16 */ int ctr, /* 0x20(sp) */ GlyphClass *glyph2, /* 0x24(sp) */ int radius, /* 0x28(sp) */ int joinType, /* 0x2c(sp) */ int capType, /* 0x30(sp) */ int open) {
	/* s2 18 */ int point;
	/* a1 5 */ int count;
	/* s4 20 */ int x1;
	/* s6 22 */ int y1;
	/* s5 21 */ int x2;
	/* s7 23 */ int y2;
	/* 0x34(sp) */ PathClass *path;
	/* 0x38(sp) */ int overlap;
	/* t3 11 */ int pointNum2;
	/* v0 2 */ int point3;
	/* a2 6 */ int x0;
	/* a3 7 */ int y0;
	/* s0 16 */ int x3;
	/* s1 17 */ int y3;
	/* v0 2 */ int point3;
	/* a0 4 */ int x3;
	/* v1 3 */ int y3;
}

/* 0016d768 0000004c */ static void YShiftGlyph(/* a0 4 */ GlyphClass *glyph, /* a1 5 */ int shift) {
	/* a0 4 */ int i;
	/* v0 2 */ int y;
	/* v0 2 */ int endPoint;
	/* a2 6 */ short int *ooy;
}

/* 0016d7b8 000002e8 */ static void ApplyHintsToStrokeGlyph(/* fp 30 */ GlyphClass *glyph, /* 0x80(sp) */ F16Dot16 xScale, /* 0x84(sp) */ F16Dot16 yScale, /* 0x88(sp) */ int radius, /* 0x8c(sp) */ int alwaysOn) {
	/* s4 20 */ int ptA;
	/* s1 17 */ int ptB;
	/* t1 9 */ int endPoint;
	/* a3 7 */ int i1;
	/* s0 16 */ short int zA;
	/* s5 21 */ short int zB;
	/* t2 10 */ short int *ooz;
	/* t3 11 */ short int pixSize;
	/* s1 17 */ int pass;
	/* a1 5 */ int ctr;
	/* s7 23 */ F16Dot16 scale;
	/* s0 16 */ F26Dot6 strokeDiameter;
	/* s6 22 */ F26Dot6 alignment;
	/* 0x0(sp) */ uint32 usageBuffer[32];
	/* s3 19 */ uint8 *usage;
	/* a1 5 */ int z2;
	/* a2 6 */ int z1;
	/* v0 2 */ short int delta;
}

/* 0016daa0 000001fc */ GlyphClass* ff_glyph_StrokeGlyph(/* s2 18 */ GlyphClass *glyph1, /* s6 22 */ int radius, /* s5 21 */ int joinType, /* s4 20 */ int capType, /* s3 19 */ int open) {
	/* s0 16 */ int ctr;
	/* a3 7 */ int pointCount1;
	/* a2 6 */ int pointCount2;
	/* s1 17 */ GlyphClass *glyph2;
}

/* 0016dca0 000000fc */ void ff_Read2Numbers(/* a2 6 */ InputStream *in, /* t2 10 */ uint16 *arr) {
	/* t1 9 */ int i;
	/* a3 7 */ int N;
	/* v1 3 */ int more;
	/* 0x0(sp) */ uint8 aData[8];
}

/* 0016dda0 00000154 */ void ff_Read4Numbers(/* a2 6 */ InputStream *in, /* t4 12 */ uint16 *arr) {
	/* t3 11 */ int i;
	/* a3 7 */ int N;
	/* v1 3 */ int more;
	/* 0x0(sp) */ uint8 aData[8];
}

/* 0016def8 0000004c */ static unsigned int ReadLowUnsignedNumber(/* a0 4 */ InputStream *in, /* a1 5 */ void *entropycoder) {
	/* a1 5 */ unsigned char value;
	/* a3 7 */ unsigned int n;
	/* a2 6 */ unsigned int shift;
}

/* 0016df48 000000f8 */ static void AllocatePointSpaceIfNeeded(/* s0 16 */ GlyphClass *t, /* a1 5 */ int N) {
	/* a0 4 */ short int *oox;
	/* v1 3 */ short int *ooy;
	/* s3 19 */ short int *ooxOld;
	/* s2 18 */ short int *ooyOld;
	/* t4 12 */ uint8 *onCurve;
	/* s4 20 */ uint8 *onCurveOld;
	/* s1 17 */ F26Dot6 *memBase;
	/* a2 6 */ int i;
	/* t3 11 */ int limit;
}

/* 0016e040 00000020 */ static void AllocateContourSpaceIfNeeded(/* a0 4 */ GlyphClass *t, /* a1 5 */ int N) {}

/* 0016e060 0000010c */ static InputStream* GetGlyphStream(/* s2 18 */ sfntClass *font, /* s4 20 */ int index, /* s7 23 */ uint32 *lengthPtr) {
	/* s0 16 */ InputStream *stream;
	/* s3 19 */ slocClass *sloc;
	/* s6 22 */ sfnt_DirectoryEntry *dirEntry;
	/* s5 21 */ unsigned int length;
	/* s1 17 */ unsigned int position;
	/* s0 16 */ unsigned int offset1;
}

/* 0016e170 00000128 */ ffhmClass* FF_New_ffhmClass(/* s0 16 */ tsiMemObject *mem, /* s3 19 */ InputStream *in) {
	/* s1 17 */ int32 i;
	/* s2 18 */ int32 N;
	/* s2 18 */ int32 version;
	/* s4 20 */ ffhmClass *t;
}

/* 0016e298 00000070 */ uint16 FF_GetAW_ffhmClass(/* a0 4 */ void *param1, /* a1 5 */ uint16 index) {
	/* t2 10 */ int low;
	/* a2 6 */ int mid;
	/* t0 8 */ int high;
	/* v0 2 */ uint16 midIndex;
	/* t1 9 */ uint16 value;
	/* t3 11 */ uint16 *gIndex;
}

/* 0016e308 0000003c */ void FF_Delete_ffhmClass(/* s0 16 */ ffhmClass *t) {}

/* 0016e348 000007ac */ GlyphClass* ff_New_GlyphClassT2KS(/* 0x48(sp) */ sfntClass *font, /* s1 17 */ GlyphClass *glyph, /* s0 16 */ int index, /* 0x4c(sp) */ uint16 *aWidth, /* 0x50(sp) */ uint16 *aHeight, /* s5 21 */ void *model, /* 0x54(sp) */ int depth) {
	/* s2 18 */ int i;
	/* s0 16 */ int iOut;
	/* a0 4 */ int j;
	/* fp 30 */ tsiMemObject *mem;
	/* 0x58(sp) */ short int *oox;
	/* 0x5c(sp) */ short int *ooy;
	/* 0x60(sp) */ uint8 *onCurve;
	/* s2 18 */ int pointCount;
	/* fp 30 */ short int xmin;
	/* s7 23 */ short int ymax;
	/* 0x0(sp) */ uint16 arr[4];
	/* v0 2 */ uint16 num0011;
	/* s3 19 */ uint16 numXX11;
	/* s6 22 */ uint16 numXXXX;
	/* s7 23 */ uint16 numCTRS;
	/* s0 16 */ short int stmp;
	/* 0x64(sp) */ InputStream *in;
	/* 0x44(sp) */ uint32 glyphLength;
	/* 0x10(sp) */ uint16 aWidth;
	/* 0x12(sp) */ uint16 aHeight;
	/* 0x18(sp) */ uint16 aWidth;
	/* 0x1a(sp) */ uint16 aHeight;
	/* 0x14(sp) */ int16 dx;
	/* 0x16(sp) */ int16 dy;
	/* s0 16 */ int countA;
	/* v1 3 */ int countB;
	/* 0x24(sp) */ uint16 aWidth;
	/* 0x26(sp) */ uint16 aHeight;
	/* 0x1c(sp) */ int16 dxO;
	/* 0x1e(sp) */ int16 dyO;
	/* 0x20(sp) */ int16 dxS;
	/* 0x22(sp) */ int16 dyS;
	/* s4 20 */ int16 xScale4Dot12;
	/* s3 19 */ int16 yScale4Dot12;
	/* s0 16 */ int countA;
	/* v1 3 */ int countB;
	/* 0x30(sp) */ uint16 arr[2];
	/* s5 21 */ short int x;
	/* s6 22 */ short int y;
	/* 0x42(sp) */ short int dx;
	/* 0x40(sp) */ short int dy;
	/* a2 6 */ int xMid;
	/* s2 18 */ int radius;
	/* s0 16 */ int diameter;
}

/* 0016eaf8 00000024 */ static void Set_ffstDefaults(/* a0 4 */ ffstClass *t) {}

/* 0016eb20 0000011c */ ffstClass* FF_New_ffstClass(/* s0 16 */ tsiMemObject *mem, /* s2 18 */ InputStream *in, /* s4 20 */ unsigned int length) {
	/* s1 17 */ ffstClass *t;
	/* s3 19 */ unsigned int i;
	/* a1 5 */ int16 value;
}

/* 0016ec40 00000024 */ void FF_Delete_ffstClass(/* a1 5 */ ffstClass *t) {}

// *****************************************************************************
// FILE -- cmn/ssysinit.c
// *****************************************************************************

/* 0016ec68 000001f0 */ SNDSYS_getopts() {}
/* 0016ee58 00000194 */ SNDSYS_setopts() {}
/* 0016eff0 00000388 */ SNDSYSI_init() {}
/* 0016f378 00000120 */ SNDSYS_restore() {}
/* 0016f498 0000000c */ SNDSYS_inited() {}

// *****************************************************************************
// FILE -- cmn/ssysreal.c
// *****************************************************************************

/* 0016f4a8 00000020 */ SNDREAL_systemtask() {}
/* 0016f4c8 00000008 */ SNDREAL_abortmsg() {}
/* 0016f4d0 00000064 */ SNDSYS_vectortoreal() {}

// *****************************************************************************
// FILE -- cmn/sserver.c
// *****************************************************************************

/* 0016f538 0000005c */ SNDSYSI_variabletimerservice() {}
/* 0016f598 000000ac */ SNDSYSI_timerservice() {}
/* 0016f648 00000284 */ SNDSYSI_100hzserver() {}
/* 0016f8d0 0000001c */ Snd::DefaultMutexLockFn() {}
/* 0016f8f0 0000001c */ Snd::DefaultMutexUnlockFn() {}
/* 0016f910 00000038 */ SNDSYS_entercritical() {}
/* 0016f948 00000038 */ SNDSYS_leavecritical() {}
/* 0016f980 00000020 */ static __static_initialization_and_destruction_0() {}
/* 0016f9a0 00000020 */ global constructors keyed to cmn_sserver.cXpbaaa() {}

// *****************************************************************************
// FILE -- cmn/smemman.c
// *****************************************************************************

/* 0016f9c0 00000030 */ SNDMEMI_constrain() {}
/* 0016f9f0 00000070 */ SNDMEMI_init() {}
/* 0016fa60 0000001c */ SNDMEMI_restore() {}
/* 0016fa80 00000238 */ SNDMEMI_allocz() {}
/* 0016fcb8 000000c8 */ SNDMEMI_free() {}
/* 0016fd80 00000058 */ SNDMEMI_display() {}

// *****************************************************************************
// FILE -- cmn/sbadd.c
// *****************************************************************************

/* 0016fdd8 000000fc */ SNDBANKI_userdatacallback() {}
/* 0016fed8 0000038c */ SNDbankadd() {}

// *****************************************************************************
// FILE -- cmn/sbremove.c
// *****************************************************************************

/* 00170268 000001d8 */ SNDbankremove() {}

// *****************************************************************************
// FILE -- cmn/sbvalid.c
// *****************************************************************************

/* 00170440 00000050 */ SNDBANKI_valid() {}

// *****************************************************************************
// FILE -- cmn/sresopat.c
// *****************************************************************************

/* 00170490 00000098 */ SNDBANKI_asyncresolvepatch() {}

// *****************************************************************************
// FILE -- cmn/sbplay.c
// *****************************************************************************

/* 00170528 00000098 */ SNDBANK_play() {}

// *****************************************************************************
// FILE -- cmn/seffect.c
// *****************************************************************************

/* 001705c0 00000078 */ SNDCTRLI_getfxbus() {}
/* 00170638 00000024 */ SNDCTRLI_fxbustoplayloc() {}
/* 00170660 000000e4 */ SNDfxinitbus() {}
/* 00170748 00000140 */ SNDfxmasterlevel() {}

// *****************************************************************************
// FILE -- cmn/sfxout.c
// *****************************************************************************

/* 00170888 0000004c */ SNDCTRL_fxoutputbalance() {}

// *****************************************************************************
// FILE -- cmn/sfxlevel.c
// *****************************************************************************

/* 001708d8 000000ac */ SNDfxlevel() {}

// *****************************************************************************
// FILE -- cmn/sstop.c
// *****************************************************************************

/* 00170988 00000058 */ SNDstop() {}

// *****************************************************************************
// FILE -- cmn/sstopall.c
// *****************************************************************************

/* 001709e0 0000007c */ SNDstopall() {}

// *****************************************************************************
// FILE -- cmn/splysdef.c
// *****************************************************************************

/* 00170a60 00000054 */ SNDplaysetdef() {}

// *****************************************************************************
// FILE -- cmn/salloc.c
// *****************************************************************************

/* 00170ab8 00000038 */ SNDVOICEI_isreserved() {}
/* 00170af0 000004a0 */ SNDVOICEI_alloc() {}
/* 00170f90 00000194 */ SNDVOICEI_free() {}
/* 00171128 00000058 */ SNDVOICEI_get() {}

// *****************************************************************************
// FILE -- cmn/scalcfx.c
// *****************************************************************************

/* 00171180 00000070 */ SNDI_calcfxlevel() {}

// *****************************************************************************
// FILE -- cmn/slib.c
// *****************************************************************************

/* 001711f0 000000d0 */ iSNDcalcvol() {}

// *****************************************************************************
// FILE -- cmn/sclcptch.c
// *****************************************************************************

/* 001712c0 000000ac */ iSNDdetunetolinear() {}
/* 00171370 000000c4 */ iSNDcalcpitch() {}

// *****************************************************************************
// FILE -- cmn/spatkey.c
// *****************************************************************************

/* 00171438 000000c0 */ iSNDpatchkey() {}

// *****************************************************************************
// FILE -- cmn/stpparse.c
// *****************************************************************************

/* 001714f8 00000608 */ SNDI_parsetimbre() {}

// *****************************************************************************
// FILE -- cmn/stagpat.c
// *****************************************************************************

/* 00171b00 00000074 */ SNDBANKI_findfreekey() {}
/* 00171b78 00000580 */ SNDBANKI_playtimbre() {}
/* 001720f8 00000298 */ SNDBANKI_playpatch() {}

// *****************************************************************************
// FILE -- cmn/satospkr.c
// *****************************************************************************

/* 00172390 0000008c */ SNDI_equalpower() {}
/* 00172420 00000440 */ SNDI_precalcaztospkrvol() {}
/* 00172860 00000088 */ SNDI_aztospkrvol() {}
/* 001728e8 0000003c */ SNDI_freespkrtable() {}

// *****************************************************************************
// FILE -- cmn/spantoaz.c
// *****************************************************************************

/* 00172928 00000018 */ SNDI_pantoazimuth() {}

// *****************************************************************************
// FILE -- cmn/srandom.c
// *****************************************************************************

/* 00172940 0000003c */ SNDI_randomseed() {}
/* 00172980 000000ec */ iSNDrandom() {}

// *****************************************************************************
// FILE -- cmn/sndi_sin.c
// *****************************************************************************

/* 00172a70 000000cc */ SNDI_sin() {}

// *****************************************************************************
// FILE -- cmn/sndi_cos.c
// *****************************************************************************

/* 00172b40 000000cc */ SNDI_cos() {}

// *****************************************************************************
// FILE -- cmn/sexithndl.c
// *****************************************************************************

/* 00172c10 0000001c */ SNDREAL_exithandler() {}

// *****************************************************************************
// FILE -- cmn/sdownmix.cpp
// *****************************************************************************

/* 00172c30 00000084 */ Snd::Util::SetDefaultAzimuths() {}

// *****************************************************************************
// FILE -- cmn/dma.cpp
// *****************************************************************************

/* 00172cb8 0000012c */ Snd::Dma::Init() {}
/* 00172de8 000000c0 */ Snd::Dma::Restore() {}
/* 00172ea8 000002b0 */ Snd::Dma::QueueRequest() {}
/* 00173158 0000008c */ Snd::Dma::IsComplete() {}
/* 001731e8 00000008 */ Snd::Dma::GetMaxDMALength() {}
/* 001731f0 000000ac */ Snd::Dma::CompletionHandler() {}
/* 001732a0 00000198 */ Snd::Dma::DmaThread() {}
/* 00173438 0000005c */ static __static_initialization_and_destruction_0() {}
/* 00173498 00000020 */ global constructors keyed to cmn_dma.cpplrbaaa() {}

// *****************************************************************************
// FILE -- ps2/sdriver.c
// *****************************************************************************

/* 001734b8 0000000c */ SND_UseNewDmaManager() {}
/* 001734c8 000000ac */ SNDSYSI_setmacflags() {}
/* 00173578 000000a0 */ SNDSYSI_savevu0registers() {}
/* 00173618 000000bc */ SNDSYSI_restorevu0registers() {}
/* 001736d8 000000a8 */ SNDDRV_savescratchpad() {}
/* 00173780 000000b0 */ SNDDRV_restorescratchpad() {}
/* 00173830 000000d4 */ SNDDRV_rpc() {}
/* 00173908 00000030 */ SNDDRV_mixvoicefree() {}
/* 00173938 00000078 */ SNDPLATFORM_getvoicerange() {}
/* 001739b0 00000124 */ SNDPLATFORM_outputcaps() {}
/* 00173ad8 000003f8 */ SNDPLATFORM_outputset() {}
/* 00173ed0 000002dc */ SNDDRV_updatevoices() {}
/* 001741b0 000000b4 */ SNDDRV_iopinterrupt() {}
/* 00174268 0000005c */ SNDDRV_strcat() {}
/* 001742c8 00000344 */ SNDDRV_resettagbuf() {}
/* 00174610 00000020 */ SNDDRV_vblankcb() {}
/* 00174630 000001dc */ SNDDRV_thread() {}
/* 00174810 00000738 */ SNDPLATFORM_init() {}
/* 00174f48 00000200 */ SNDPLATFORM_restore() {}
/* 00175148 00000008 */ iSNDserve() {}
/* 00175150 0000019c */ SNDPLATFORM_stop() {}
/* 001752f0 000001fc */ SNDPLATFORM_setpitch() {}
/* 001754f0 0000026c */ SNDPLATFORM_setfxlevel() {}
/* 00175760 00000270 */ Snd::Hal::SetVol() {}
/* 001759d0 000000fc */ Snd::Hal::SetDry() {}
/* 00175ad0 000000b4 */ SNDPLATFORM_timemult() {}
/* 00175b88 000000b0 */ SNDPLATFORM_setdistortlevel() {}
/* 00175c38 000000d8 */ SNDPLATFORM_lowpass() {}
/* 00175d10 000000b0 */ SNDPLATFORM_highpass() {}
/* 00175dc0 00000058 */ SNDI_mutexalloc() {}
/* 00175e18 00000028 */ SNDI_mutexfree() {}
/* 00175e40 00000060 */ SNDI_mutexlock() {}
/* 00175ea0 00000070 */ SNDI_mutexunlock() {}

// *****************************************************************************
// FILE -- ps2/sdfx.c
// *****************************************************************************

/* 00175f10 000000b0 */ SNDDRV_setiopfx() {}
/* 00175fc0 000000f8 */ SNDDRV_setspufx() {}
/* 001760b8 000000b0 */ SNDDRV_setfx() {}
/* 00176168 00000020 */ SNDPLATFORM_fxinit() {}
/* 00176188 00000060 */ SNDPLATFORM_fxoutputbalance() {}

// *****************************************************************************
// FILE -- ps2/sdrespat.c
// *****************************************************************************

/* 001761e8 00000038 */ SNDPLATFORM_asyncresolvetimbre() {}

// *****************************************************************************
// FILE -- ps2/sdplapat.c
// *****************************************************************************

/* 00176220 000004c4 */ SNDPLATFORM_playtimbre() {}

// *****************************************************************************
// FILE -- ps2/sdasync.c
// *****************************************************************************

/* 001766e8 0000001c */ SNDPLATFORM_download() {}
/* 00176708 0000001c */ SNDPLATFORM_downloadcomplete() {}
/* 00176728 0000008c */ SNDPLATFORM_downloadNew() {}
/* 001767b8 00000044 */ SNDPLATFORM_downloadcompleteNew() {}

// *****************************************************************************
// FILE -- ps2/sdma.c
// *****************************************************************************

/* 00176800 0000008c */ SNDDRVI_dmeetoiopxfer() {}
/* 00176890 00000194 */ SNDDRV_dmtransfer() {}
/* 00176a28 0000007c */ SNDDRV_dmservice() {}
/* 00176aa8 0000011c */ SNDDRV_dmqueue() {}
/* 00176bc8 0000010c */ SNDDRV_dmqueuesplit() {}
/* 00176cd8 00000080 */ SNDDRV_dmiscomplete() {}
/* 00176d58 00000060 */ SNDDRVI_dmcompleterequest() {}

// *****************************************************************************
// FILE -- ps2/sdmaee.c
// *****************************************************************************

/* 00176db8 000000bc */ SNDDMAI_ToScratchPad() {}
/* 00176e78 000000bc */ SNDDMAI_FromScratchPad() {}

// *****************************************************************************
// FILE -- ps2/sdmemman.c
// *****************************************************************************

/* 00176f38 00000078 */ SNDDRV_memconstrain() {}
/* 00176fb0 00000204 */ SNDPLATFORM_memalloc() {}
/* 001771b8 000000d4 */ SNDPLATFORM_memfree() {}

// *****************************************************************************
// FILE -- ps2/smemcpy.s
// *****************************************************************************

/* 0017728c 00000110 */ SNDI_memcpy() {}

// *****************************************************************************
// FILE -- cmn/ssys.cpp
// *****************************************************************************

/* 001773a0 00000048 */ Snd::System::SetMaxBanks() {}
/* 001773e8 0000003c */ Snd::System::GetMaxBanks() {}
/* 00177428 00000048 */ Snd::System::SetMaxStreams() {}
/* 00177470 00000048 */ Snd::System::SetStealEqualPriorityVoices() {}
/* 001774b8 00000048 */ Snd::System::SetRandomSeed() {}
/* 00177500 00000064 */ Snd::System::SetUpdateRate() {}
/* 00177568 0000005c */ Snd::System::GetUpdateRate() {}
/* 001775c8 00000080 */ Snd::System::CapOutputMode() {}
/* 00177648 000000c4 */ Snd::System::GetOutputModeName() {}
/* 00177710 00000044 */ Snd::System::SetOutputMode() {}
/* 00177758 00000014 */ Snd::System::GetOutputMode() {}
/* 00177770 000000b8 */ Snd::System::CapOutputSampleRate() {}
/* 00177828 00000080 */ Snd::System::SetOutputSampleRate() {}
/* 001778a8 00000070 */ Snd::System::GetOutputSampleRate() {}
/* 00177918 00000098 */ Snd::System::CapVoices() {}
/* 001779b0 00000098 */ Snd::System::SetVoices() {}
/* 00177a48 00000094 */ Snd::System::GetVoices() {}
/* 00177ae0 00000008 */ Snd::System::SetCompatibilityLevel() {}
/* 00177ae8 0000009c */ Snd::System::SetResampleQuality() {}
/* 00177b88 00000080 */ Snd::System::GetResampleQuality() {}
/* 00177c08 00000008 */ Snd::System::CapEax() {}
/* 00177c10 00000008 */ Snd::System::SetEax() {}
/* 00177c18 00000008 */ Snd::System::GetEax() {}
/* 00177c20 00000008 */ Snd::System::SetSse() {}
/* 00177c28 00000048 */ Snd::System::SetSndLoadsIopModules() {}
/* 00177c70 00000048 */ Snd::System::SetIopModulePath() {}
/* 00177cb8 0000005c */ Snd::System::SetDtsQuality() {}
/* 00177d18 00000048 */ Snd::System::SetRouteMainFxToIop() {}
/* 00177d60 00000048 */ Snd::System::SetRouteHardwareFxToIop() {}
/* 00177da8 00000008 */ Snd::System::SetSndInitsAram() {}
/* 00177db0 00000048 */ Snd::System::SetAbortHandler() {}
/* 00177df8 000002b0 */ Snd::System::Init() {}
/* 001780a8 00000014 */ Snd::System::IsInited() {}
/* 001780c0 00000028 */ Snd::System::Restore() {}
/* 001780e8 00000024 */ Snd::Memory::SetHeap() {}
/* 00178110 00000074 */ Snd::Memory::GetHeap() {}
/* 00178188 00000058 */ Snd::Memory::SetHeapFastRam() {}
/* 001781e0 0000006c */ Snd::Memory::SetHeapThreshold() {}
/* 00178250 00000068 */ Snd::Memory::GetHeapThreshold() {}
/* 001782b8 00000058 */ Snd::Profile::SetFunctionProfiler() {}

// *****************************************************************************
// FILE -- cmn/ssysserv.c
// *****************************************************************************

/* 00178310 00000028 */ iSNDserveraddclient() {}
/* 00178338 0000009c */ iSNDserverremoveclient() {}
/* 001783d8 00000074 */ SNDSYS_service() {}

// *****************************************************************************
// FILE -- cmn/srender.c
// *****************************************************************************

/* 00178450 000000e8 */ SNDI_validrendermode() {}

// *****************************************************************************
// FILE -- cmn/smemhigh.cpp
// *****************************************************************************

/* 00178538 00000034 */ SNDMEM_gethighwater() {}

// *****************************************************************************
// FILE -- cmn/smemdis.c
// *****************************************************************************

/* 00178570 0000002c */ SNDMEM_display() {}
/* 001785a0 0000001c */ SNDMEM_report() {}

// *****************************************************************************
// FILE -- cmn/sballoc.c
// *****************************************************************************

/* 001785c0 00000040 */ SNDBANKI_alloc() {}
/* 00178600 0000003c */ SNDBANKI_getppatch() {}

// *****************************************************************************
// FILE -- cmn/spktplay.c
// *****************************************************************************

/* 00178640 00000060 */ SNDPKTPLAYI_gethighchannel() {}
/* 001786a0 0000000c */ SNDPKTPLAYI_overhead() {}
/* 001786b0 00000030 */ SNDPKTPLAY_overhead() {}
/* 001786e0 00000140 */ SNDPKTPLAY_create() {}
/* 00178820 00000454 */ SNDPKTPLAY_start() {}
/* 00178c78 0000016c */ SNDPKTPLAY_submit() {}
/* 00178de8 00000070 */ SNDPKTPLAY_submitspace() {}
/* 00178e58 00000024 */ SNDPKTPLAY_framesoutstanding() {}
/* 00178e80 00000020 */ SNDPKTPLAY_hardwareframesoutstanding() {}
/* 00178ea0 00000008 */ SNDPKTPLAY_purge() {}
/* 00178ea8 000000b4 */ SNDPKTPLAY_stop() {}
/* 00178f60 00000048 */ SNDPKTPLAY_destroy() {}
/* 00178fa8 00000230 */ SNDPKTPLAYI_get() {}
/* 001791d8 0000006c */ SNDPKTPLAYI_freeframes() {}
/* 00179248 000000b8 */ SNDPKTPLAYI_flushcallbackdata() {}

// *****************************************************************************
// FILE -- cmn/sover.c
// *****************************************************************************

/* 00179300 00000020 */ SNDover() {}

// *****************************************************************************
// FILE -- cmn/srrange.c
// *****************************************************************************

/* 00179320 00000058 */ randrange() {}

// *****************************************************************************
// FILE -- cmn/sgetdata.c
// *****************************************************************************

/* 00179378 0000008c */ SNDI_getb() {}

// *****************************************************************************
// FILE -- cmn/sgettag.c
// *****************************************************************************

/* 00179408 000000f4 */ SNDI_gettag() {}

// *****************************************************************************
// FILE -- cmn/sformat.c
// *****************************************************************************

/* 00179500 0000007c */ SNDMEMI_printf() {}
/* 00179580 000001b0 */ SNDMEMI_format() {}

// *****************************************************************************
// FILE -- mix/smixer.c
// *****************************************************************************

/* 00179730 00000228 */ MIXI_interpolateto0() {}
/* 00179958 0000027c */ MIXI_interpolatemix() {}
/* 00179bd8 00000008 */ SNDMIX_setmasterlowpass() {}
/* 00179be0 0000020c */ SNDMIXI_volramp() {}
/* 00179df0 0000001c */ SNDI_New() {}
/* 00179e10 0000001c */ SNDI_Delete() {}
/* 00179e30 00000318 */ MIX_create() {}
/* 0017a148 0000010c */ MIX_destroy() {}
/* 0017a258 00000020 */ MIX_reset() {}
/* 0017a278 0000000c */ MIX_setresamplequality() {}
/* 0017a288 00000244 */ MIX_playinit() {}
/* 0017a4d0 00000090 */ MIX_play() {}
/* 0017a560 00000034 */ MIXI_stop() {}
/* 0017a598 000000f8 */ MIX_stop() {}
/* 0017a690 00000038 */ SNDMIX_setdrygain() {}
/* 0017a6c8 00000038 */ SNDMIX_setwetgain() {}
/* 0017a700 00000398 */ MIX_audioslice() {}
/* 0017aa98 000000d4 */ MIX_audio() {}

// *****************************************************************************
// FILE -- mix/smixptch.c
// *****************************************************************************

/* 0017ab70 000000b8 */ MIX_setpitch() {}

// *****************************************************************************
// FILE -- mix/smixtmul.c
// *****************************************************************************

/* 0017ac28 0000003c */ MIX_settimemult() {}

// *****************************************************************************
// FILE -- mix/smixdist.c
// *****************************************************************************

/* 0017ac68 000000b0 */ MIX_setdistortlevel() {}

// *****************************************************************************
// FILE -- mix/smixhip.c
// *****************************************************************************

/* 0017ad18 000000d0 */ MIX_sethighpass() {}

// *****************************************************************************
// FILE -- mix/smixlowp.c
// *****************************************************************************

/* 0017ade8 00000124 */ MIX_setlowpass() {}

// *****************************************************************************
// FILE -- mix/sfilter.c
// *****************************************************************************

/* 0017af10 00000068 */ SFILTER_add() {}
/* 0017af78 0000005c */ SFILTER_remove() {}
/* 0017afd8 0000002c */ SFILTER_destroy() {}
/* 0017b008 0000007c */ SFILTER_cleanup() {}
/* 0017b088 000000c8 */ SFILTER_connect() {}
/* 0017b150 000000dc */ SFILTER_disconnect() {}

// *****************************************************************************
// FILE -- mix/sinit16.c
// *****************************************************************************

/* 0017b230 00000048 */ MIXI_initunpack16() {}

// *****************************************************************************
// FILE -- mix/sinitxa.c
// *****************************************************************************

/* 0017b278 00000048 */ MIXI_initunpackxa() {}

// *****************************************************************************
// FILE -- mix/sinitut.c
// *****************************************************************************

/* 0017b2c0 00000060 */ MIXI_initunpackmt() {}

// *****************************************************************************
// FILE -- mix/supmutf.c
// *****************************************************************************

/* 0017b320 00000008 */ SFILTER_unpackgetframemtf() {}
/* 0017b328 00000094 */ SFILTER_unpackmtf() {}
/* 0017b3c0 00000028 */ SFILTER_unpackmtfrestore() {}
/* 0017b3e8 000000f4 */ SFILTER_unpackmtfinit() {}

// *****************************************************************************
// FILE -- mix/supmutlf.c
// *****************************************************************************

/* 0017b4e0 00000008 */ SFILTER_unpackgetframemtlf() {}
/* 0017b4e8 000000d0 */ SFILTER_unpackmtlf() {}
/* 0017b5b8 00000028 */ SFILTER_unpackmtlfrestore() {}
/* 0017b5e0 000000d4 */ SFILTER_unpackmtlfinit() {}

// *****************************************************************************
// FILE -- mix/supmutpf.c
// *****************************************************************************

/* 0017b6b8 00000248 */ SFILTER_unpackmtpf() {}
/* 0017b900 00000028 */ SFILTER_unpackmtpfrestore() {}
/* 0017b928 00000094 */ SFILTER_unpackmtpfinit() {}

// *****************************************************************************
// FILE -- mix/supf.c
// *****************************************************************************

/* 0017b9c0 00000008 */ SFILTER_unpackfgetframe() {}
/* 0017b9c8 000000c4 */ SFILTER_unpackf() {}
/* 0017ba90 0000003c */ SFILTER_unpackfinit() {}

// *****************************************************************************
// FILE -- mix/suplf.c
// *****************************************************************************

/* 0017bad0 00000008 */ SFILTER_unpacklfgetframe() {}
/* 0017bad8 000000b8 */ SFILTER_unpacklf() {}
/* 0017bb90 00000040 */ SFILTER_unpacklfinit() {}

// *****************************************************************************
// FILE -- mix/suppf.c
// *****************************************************************************

/* 0017bbd0 00000120 */ SFILTER_unpackpf() {}
/* 0017bcf0 00000074 */ SFILTER_unpackpfinit() {}

// *****************************************************************************
// FILE -- mix/sx87d16.c
// *****************************************************************************

/* 0017bd68 000000ac */ SNDI_s16tofunaligned() {}
/* 0017be18 00000108 */ decode16x87() {}

// *****************************************************************************
// FILE -- mix/supxaf.cpp
// *****************************************************************************

/* 0017bf20 000000a0 */ SFILTER_unpackxaf() {}
/* 0017bfc0 00000008 */ SFILTER_unpackgetframexaf() {}
/* 0017bfc8 00000028 */ SFILTER_unpackxafrestore() {}
/* 0017bff0 00000090 */ SFILTER_unpackxafinit() {}

// *****************************************************************************
// FILE -- mix/supxalf.cpp
// *****************************************************************************

/* 0017c080 0000029c */ SFILTER_unpackxalf() {}
/* 0017c320 00000008 */ SFILTER_unpackgetframexalf() {}
/* 0017c328 00000028 */ SFILTER_unpackxalfrestore() {}
/* 0017c350 000000d8 */ SFILTER_unpackxalfinit() {}

// *****************************************************************************
// FILE -- mix/supxapf.cpp
// *****************************************************************************

/* 0017c428 00000188 */ SFILTER_unpackxapf() {}
/* 0017c5b0 00000028 */ SFILTER_unpackxapfrestore() {}
/* 0017c5d8 00000094 */ SFILTER_unpackxapfinit() {}

// *****************************************************************************
// FILE -- mix/sfft16.c
// *****************************************************************************

/* 0017c670 000000f8 */ SFILTER_ft16() {}
/* 0017c768 00000014 */ SFILTER_ft16init() {}

// *****************************************************************************
// FILE -- mix/sflpf.c
// *****************************************************************************

/* 0017c780 000000b4 */ SFILTER_lpfRC() {}
/* 0017c838 00000030 */ SFILTER_createLPFRC() {}
/* 0017c868 00000060 */ SFILTER_modifyLPFRC() {}

// *****************************************************************************
// FILE -- mix/sfhpffir8.c
// *****************************************************************************

/* 0017c8c8 0000008c */ SFILTER_hpfFIR8() {}
/* 0017c958 00000048 */ SFILTER_createHPFFIR8() {}
/* 0017c9a0 0000004c */ SFILTER_modifyHPFFIR8() {}

// *****************************************************************************
// FILE -- mix/stretch.c
// *****************************************************************************

/* 0017c9f0 000000fc */ static crossfade() {}
/* 0017caf0 0000015c */ static determineaction() {}
/* 0017cc50 00000168 */ static applyaction() {}
/* 0017cdb8 000000ac */ static transferframes() {}
/* 0017ce68 00000130 */ stretchframesneeded() {}
/* 0017cf98 00000140 */ stretch() {}
/* 0017d0d8 00000094 */ SFILTER_timestretchsetratio() {}
/* 0017d170 00000110 */ SFILTER_timestretch() {}
/* 0017d280 000000a8 */ SFILTER_timestretchinit() {}

// *****************************************************************************
// FILE -- mix/sfdist.c
// *****************************************************************************

/* 0017d328 00000180 */ SFILTER_distort() {}
/* 0017d4a8 0000007c */ SFILTER_distortsetlevel() {}
/* 0017d528 00000010 */ SFILTER_distortinit() {}

// *****************************************************************************
// FILE -- mix/sfrsf.c
// *****************************************************************************

/* 0017d538 00000008 */ SFILTER_rsfsetpitch() {}
/* 0017d540 000005e4 */ SFILTER_rsf() {}
/* 0017db28 0000004c */ SFILTER_rsfinit() {}

// *****************************************************************************
// FILE -- mix/smixc.c
// *****************************************************************************

/* 0017db78 00000088 */ mixc() {}

// *****************************************************************************
// FILE -- mix/sfxrevc.c
// *****************************************************************************

/* 0017dc00 0000000c */ MIX_setwetbuffer() {}
/* 0017dc10 0000000c */ MIX_getwetbuffer() {}
/* 0017dc20 00000038 */ remap() {}
/* 0017dc58 00000270 */ SNDMIXI_modlapifxadd() {}
/* 0017dec8 000001a8 */ SNDMIXI_restorefx2() {}
/* 0017e070 000005bc */ SNDMIXI_initfx() {}
/* 0017e630 00000214 */ MIX_initreverb() {}
/* 0017e848 00000030 */ MIX_restorereverb() {}
/* 0017e878 0000009c */ SNDMIXI_fxadd() {}

// *****************************************************************************
// FILE -- ..\..\codasnd8\cmn\coda.cpp
// *****************************************************************************

/* 0017e918 0000000c */ Snd::CODASetNew() {}
/* 0017e928 0000000c */ Snd::CODASetDelete() {}

// *****************************************************************************
// FILE -- ..\..\codasnd8\cmn\eaxadecf.cpp
// *****************************************************************************

/* 0017e938 000000cc */ process_raw_block() {}
/* 0017ea08 000004e4 */ Snd::decodexac() {}
/* 0017eef0 00000024 */ Snd::CEAXABLKDecf::operator new() {}
/* 0017ef18 00000024 */ Snd::CEAXABLKDecf::operator delete() {}
/* 0017ef40 0000001c */ Snd::CEAXABLKDecf::CEAXABLKDecf() {}
/* 0017ef60 00000030 */ Snd::CEAXABLKDecf::Feed() {}
/* 0017ef90 000001a8 */ Snd::CEAXABLKDecf::Decode() {}
/* 0017f138 00000058 */ Snd::CEAXABLKDecf::GetState() {}
/* 0017f190 00000014 */ Snd::CEAXABLKDecf::SetState() {}

// *****************************************************************************
// FILE -- ..\..\codasnd8\cmn\mtdecf.cpp
// *****************************************************************************

/* 0017f1a8 00000024 */ Snd::CMTBLKDecf::operator new() {}
/* 0017f1d0 00000024 */ Snd::CMTBLKDecf::operator delete() {}
/* 0017f1f8 00000068 */ static Snd::getbits() {}
/* 0017f260 00000050 */ Snd::discardbits() {}
/* 0017f2b0 00000264 */ Snd::readsamples() {}
/* 0017f518 00000078 */ static Snd::interpolate() {}
/* 0017f590 000000fc */ static Snd::reftolpc() {}
/* 0017f690 00000060 */ static Snd::filter() {}
/* 0017f6f0 00000184 */ Snd::CMTBLKDecf::initmut() {}
/* 0017f878 00000540 */ Snd::decodemut() {}
/* 0017fdb8 0000002c */ Snd::CMTBLKDecf::CMTBLKDecf() {}
/* 0017fde8 00000080 */ Snd::CMTBLKDecf::Feed() {}
/* 0017fe68 0000020c */ Snd::CMTBLKDecf::Decode() {}
/* 00180078 00000040 */ Snd::CMTBLKDecf::GetState() {}
/* 001800b8 0000001c */ Snd::CMTBLKDecf::SetState() {}
/* 001800d8 00000008 */ Snd::CMTBLKDecf::SetCodecVersion() {}

// *****************************************************************************
// FILE -- ps2/sd3dpos.c
// *****************************************************************************

/* 001800e0 00000160 */ SNDI_dolby() {}
/* 00180240 000001ac */ Snd::Hal::SetPan() {}

// *****************************************************************************
// FILE -- ps2/sdpacket.c
// *****************************************************************************

/* 001803f0 00000038 */ SNDDRV_getmastervoice() {}
/* 00180428 0000007c */ SNDDRV_getsamplechan() {}
/* 001804a8 00000008 */ SNDPLATFORM_packetoverhead() {}
/* 001804b0 00000008 */ SNDPLATFORM_packetplaycreate() {}
/* 001804b8 00000008 */ SNDPLATFORM_packetplaydestroy() {}
/* 001804c0 0000022c */ SNDPLATFORM_packetplay() {}

// *****************************************************************************
// FILE -- ps2/sdmemdis.c
// *****************************************************************************

/* 001806f0 00000374 */ SNDPLATFORM_memdisplay() {}

// *****************************************************************************
// FILE -- cmn/spktctoh.c
// *****************************************************************************

/* 00180a68 00000098 */ SNDPKTPLAYI_voicetopackethandle() {}

// *****************************************************************************
// FILE -- mix/smtflt59.s
// *****************************************************************************

/* 00180b00 000003ac */ mtfilter5900() {}

// *****************************************************************************
// FILE -- mix/sfir.c
// *****************************************************************************

/* 00180eb0 000003cc */ calcFIRCoeffs() {}

// *****************************************************************************
// FILE -- mix/sfir8.c
// *****************************************************************************

/* 00181280 00000054 */ SNDI_fir8init() {}
/* 001812d8 0000021c */ SNDI_fir8() {}

// *****************************************************************************
// FILE -- mix/sflpffir8.c
// *****************************************************************************

/* 001814f8 00000080 */ SFILTER_lpfFIR8() {}
/* 00181578 00000048 */ SFILTER_createLPFFIR8() {}
/* 001815c0 00000050 */ SFILTER_modifyLPFFIR8() {}

// *****************************************************************************
// FILE -- mix/sfbpffir8.c
// *****************************************************************************

/* 00181610 0000008c */ SFILTER_bpfFIR8() {}
/* 001816a0 00000048 */ SFILTER_createBPFFIR8() {}
/* 001816e8 00000068 */ SFILTER_modifyBPFFIR8() {}

// *****************************************************************************
// FILE -- mix/sfamplf.c
// *****************************************************************************

/* 00181750 000000c8 */ SFILTER_amplf() {}
/* 00181818 00000030 */ SFILTER_createAMPLF() {}
/* 00181848 0000001c */ SFILTER_modifyAMPLF() {}

// *****************************************************************************
// FILE -- mix/sfmixer.c
// *****************************************************************************

/* 00181868 00000120 */ SFILTER_mixer() {}
/* 00181988 00000028 */ SFILTER_mixerrestore() {}
/* 001819b0 0000003c */ SFILTER_createMIX() {}

// *****************************************************************************
// FILE -- mix/sfreson.c
// *****************************************************************************

/* 001819f0 00000104 */ resonx87() {}
/* 00181af8 0000008c */ SFILTER_reson() {}
/* 00181b88 00000038 */ SFILTER_createRESON() {}
/* 00181bc0 000000fc */ SFILTER_modifyRESON() {}

// *****************************************************************************
// FILE -- mix/sfsplit.c
// *****************************************************************************

/* 00181cc0 00000100 */ SFILTER_splitter() {}
/* 00181dc0 00000028 */ SFILTER_splitrestore() {}
/* 00181de8 00000048 */ SFILTER_createSPLIT() {}

// *****************************************************************************
// FILE -- mix/sfsrc.c
// *****************************************************************************

/* 00181e30 00000058 */ SFILTER_src() {}
/* 00181e88 00000018 */ SFILTER_initSOURCE() {}
/* 00181ea0 00000020 */ SFILTER_createSOURCE() {}

// *****************************************************************************
// FILE -- mix/sfecho.c
// *****************************************************************************

/* 00181ec0 00000184 */ SFILTER_echo() {}
/* 00182048 0000003c */ SFILTER_echorestore() {}
/* 00182088 0000003c */ SFILTER_createECHO() {}
/* 001820c8 000000bc */ SFILTER_modifyECHO() {}

// *****************************************************************************
// FILE -- cmn/sndi_root1x.c
// *****************************************************************************

/* 00182188 0000009c */ SNDI_rootof1plusx() {}

// *****************************************************************************
// FILE -- cmn/sndi_mult16.c
// *****************************************************************************

/* 00182228 00000030 */ SNDI_findmult16() {}

// *****************************************************************************
// FILE -- cmn/sndi_findprime.c
// *****************************************************************************

/* 00182258 0000002c */ SNDI_cheapsqrt() {}
/* 00182288 00000098 */ SNDI_findprime() {}

// *****************************************************************************
// FILE -- device.cpp
// *****************************************************************************

/* 00182320 000001bc */ EAGL::Device::~Device() {}
/* 001824e0 00000258 */ EAGL::Device::Init() {}
/* 00182738 00000034 */ static __static_initialization_and_destruction_0() {}
/* 00182770 00000040 */ EAGL::Device::Device() {}
/* 001827b0 00000008 */ EAGL::Device::DumpState() {}
/* 001827b8 00000010 */ EAGL::DeviceExtension::DeviceExtension() {}
/* 001827c8 00000024 */ EAGL::DeviceExtension::~DeviceExtension() {}
/* 001827f0 0000001c */ EAGL::DeviceExtension::SetNumbDMABuffers() {}
/* 00182810 0000000c */ EAGL::DeviceExtension::GetNumbDMABuffers() {}
/* 00182820 0000000c */ EAGL::DeviceExtension::SetDMABufferLength() {}
/* 00182830 0000000c */ EAGL::DeviceExtension::GetDMABufferLength() {}
/* 00182840 0000000c */ EAGL::DeviceExtension::SetTextureBufferLength() {}
/* 00182850 0000000c */ EAGL::DeviceExtension::GetTextureBufferLength() {}
/* 00182860 00000028 */ EAGL::DeviceExtension::GetDMAUsage() {}
/* 00182888 00000028 */ EAGL::DeviceExtension::GetTextureDMAUsage() {}
/* 001828b0 0000001c */ EAGL::DeviceExtension::GetMaxDMAUsage() {}
/* 001828d0 0000002c */ EAGL::DeviceExtension::DMAPerf_TimeStamp() {}
/* 00182900 0000000c */ EAGL::DeviceExtension::SetDMAPerfLength() {}
/* 00182910 0000000c */ EAGL::DeviceExtension::GetDMAPerfLength() {}
/* 00182920 00000024 */ EAGL::DeviceExtension::CalcDMAChecksums() {}
/* 00182948 00000024 */ EAGL::DeviceExtension::VerifyDMAChecksums() {}
/* 00182970 00000024 */ EAGL::DeviceExtension::SetTextureBasePointer() {}
/* 00182998 00000024 */ EAGL::DeviceExtension::DumpTexAlloc() {}
/* 001829c0 00000108 */ EAGL::DeviceExtension::ResizeDMABuffers() {}
/* 00182ac8 00000010 */ EAGL::DeviceExtension::SetVifDmaCallback() {}
/* 00182ad8 00000010 */ EAGL::DeviceExtension::SetGifDmaCallback() {}
/* 00182ae8 00000010 */ EAGLInternal::DevicePrivate::DevicePrivate() {}
/* 00182af8 00000024 */ EAGLInternal::DevicePrivate::~DevicePrivate() {}
/* 00182b20 0000000c */ EAGL::DeviceExtension::SetDMATimeoutValue() {}
/* 00182b30 0000000c */ EAGL::DeviceExtension::GetDMATimeoutValue() {}
/* 00182b40 00000008 */ EAGL::DeviceExtension::DumpState() {}
/* 00182b48 00000008 */ EAGL::DeviceExtension::DumpSystemState() {}
/* 00182b50 0000000c */ EAGL::DeviceExtension::GetNumInterleavedTexBuffers() {}
/* 00182b60 0000002c */ EAGL::DeviceExtension::DumpInterleavedTexInfo() {}
/* 00182b90 00000020 */ global constructors keyed to device.cppWOaaaa() {}

// *****************************************************************************
// FILE -- paulvif.h
// *****************************************************************************

/* ffffffff 00000068 */ EAGLInternal::DMAAccumulator::Close() {}
/* ffffffff 00000024 */ EAGLInternal::DMAAccumulator::operator delete() {}

// *****************************************************************************
// FILE -- allocator.h
// *****************************************************************************

/* ffffffff 00000024 */ EAGLInternal::VUmuAlloc::operator delete() {}

// *****************************************************************************
// FILE -- tar.cpp
// *****************************************************************************

/* 00182bb0 000019d4 */ EAGLInternal::Constructors::RuntimeAllocTARConstructor() {}
/* 00184588 00000770 */ EAGLInternal::Constructors::TARConstructor() {}
/* 00184cf8 000000e4 */ EAGL::TARExtension::Default() {}
/* 00184de0 0000051c */ EAGL::TARExtension::Init() {}
/* 00185300 00000428 */ EAGL::TARExtension::LockInterleaved() {}
/* 00185728 000000fc */ EAGL::TARExtension::SwapClut() {}
/* 00185828 00000258 */ EAGL::TARExtension::Use() {}
/* 00185a80 0000029c */ EAGL::TARExtension::OutputGifTags() {}
/* 00185d20 00000134 */ EAGL::TARExtension::TemporaryREALMetalbinCheck() {}
/* 00185e58 0000010c */ EAGL::TARExtension::GetTRC() {}
/* 00185f68 0000012c */ EAGL::TARExtension::EnableUpload() {}
/* 00186098 00000194 */ EAGL::TARExtension::SwapTextureSettings() {}
/* 00186230 00000114 */ EAGL::TARExtension::AddBarrier() {}
/* 00186348 00000244 */ EAGL::TARExtension::UploadOverlay() {}
/* 00186590 00000284 */ EAGL::TARExtension::OutputDmaChain() {}
/* 00186818 0000052c */ EAGL::TARExtension::Init() {}
/* 00186d48 00000374 */ EAGL::TARExtension::Init() {}
/* 001870c0 00000034 */ static __static_initialization_and_destruction_0() {}
/* 001870f8 00000080 */ EAGLInternal::Constructors::RuntimeAllocTARDestructor() {}
/* 00187178 0000001c */ EAGLInternal::Constructors::TARDestructor() {}
/* 00187198 00000040 */ EAGL::TAR::TAR() {}
/* 001871d8 00000050 */ EAGL::TAR::TAR() {}
/* 00187228 00000068 */ EAGL::TAR::~TAR() {}
/* 00187290 00000050 */ EAGL::TAR::TAR() {}
/* 001872e0 00000028 */ EAGL::TAR::Lock() {}
/* 00187308 0000001c */ EAGL::TAR::Unlock() {}
/* 00187328 0000000c */ EAGL::TAR::GetShape() {}
/* 00187338 0000001c */ EAGL::TAR::SwapShape() {}
/* 00187358 0000001c */ EAGL::TAR::SwapClut() {}
/* 00187378 0000001c */ EAGL::TAR::Use() {}
/* 00187398 0000000c */ EAGL::TAR::GetRefCount() {}
/* 001873a8 00000008 */ EAGL::TAR::DumpState() {}
/* 001873b0 00000028 */ EAGL::TARExtension::TARExtension() {}
/* 001873d8 0000002c */ EAGL::TARExtension::~TARExtension() {}
/* 00187408 00000074 */ EAGL::TARExtension::Kill() {}
/* 00187480 0000005c */ EAGL::TARExtension::Init() {}
/* 001874e0 000000e8 */ EAGL::TARExtension::Init() {}
/* 001875c8 0000008c */ EAGL::TARExtension::Swap() {}
/* 00187658 00000160 */ EAGL::TARExtension::Swap() {}
/* 001877b8 00000158 */ EAGL::TARExtension::Lock() {}
/* 00187910 00000088 */ EAGL::TARExtension::SetOddSizedTexture() {}
/* 00187998 00000084 */ EAGL::TARExtension::GetVRAMSize() {}
/* 00187a20 0000007c */ EAGL::TARExtension::SetLargestLevelToUpload() {}
/* 00187aa0 00000094 */ EAGL::TARExtension::SetSmallestLevelToUpload() {}
/* 00187b38 00000064 */ EAGL::TARExtension::SetRegionRepeatMaxMinU() {}
/* 00187ba0 00000084 */ EAGL::TARExtension::SetRegionRepeatMaxMinV() {}
/* 00187c28 00000024 */ EAGL::TARExtension::SetFixedMIPMAP() {}
/* 00187c50 00000028 */ EAGL::TARExtension::SetMIPMAPParam() {}
/* 00187c78 00000024 */ EAGL::TARExtension::SetMMAG() {}
/* 00187ca0 00000024 */ EAGL::TARExtension::SetMMIN() {}
/* 00187cc8 00000094 */ EAGL::TARExtension::MakeMiniTar() {}
/* 00187d60 0000000c */ EAGL::TARExtension::DisableUpload() {}
/* 00187d70 000000f4 */ EAGL::TARExtension::CreateNoUploadTAR() {}
/* 00187e68 00000030 */ EAGL::TARExtension::UnlockAll() {}
/* 00187e98 00000024 */ EAGL::TARExtension::MakeLockMark() {}
/* 00187ec0 00000034 */ EAGL::TARExtension::UnlockToMark() {}
/* 00187ef8 0000002c */ EAGL::TARExtension::FlushAll() {}
/* 00187f28 00000020 */ EAGL::TARExtension::SetDefaultMIPMAPParams() {}
/* 00187f48 000000d0 */ EAGL::TARExtension::CalcSize() {}
/* 00188018 000000c8 */ EAGL::TARExtension::CalcClutSize() {}
/* 001880e0 00000058 */ EAGL::TARExtension::BppToPSM() {}
/* 00188138 0000004c */ EAGL::TARExtension::PSMToBpp() {}
/* 00188188 00000048 */ EAGL::TARExtension::FindClut() {}
/* 001881d0 00000008 */ EAGL::TARExtension::DebugDumpVRAM() {}
/* 001881d8 0000002c */ EAGL::TARExtension::DebugDumpVRAM() {}
/* 00188208 00000074 */ EAGLInternal::TARSharedData::TARSharedData() {}
/* 00188280 00000098 */ EAGLInternal::TARSharedData::~TARSharedData() {}
/* 00188318 000000c4 */ EAGLInternal::TARSharedData::Default() {}
/* 001883e0 0000006c */ EAGLInternal::TARSharedData::Kill() {}
/* 00188450 00000008 */ EAGLInternal::TARSharedData::Init() {}
/* 00188458 00000008 */ EAGL::TARExtension::GetOverlay() {}
/* 00188460 00000010 */ EAGL::TARExtension::SetOverlay() {}
/* 00188470 00000084 */ EAGL::TARExtension::DumpVRAMState() {}
/* 001884f8 00000008 */ EAGL::TARExtension::DumpState() {}
/* 00188500 000000a0 */ EAGL::TAR::TAR() {}
/* 001885a0 00000090 */ EAGL::TAR::TAR() {}
/* 00188630 0000000c */ EAGL::TARExtension::SetClutAddress() {}
/* 00188640 00000010 */ EAGL::TARExtension::GetClutWidth() {}
/* 00188650 00000020 */ global constructors keyed to tar.cpplKaaaa() {}

// *****************************************************************************
// FILE -- D:/release/ps2/include/ps2/eagl/runtime/tar.inl
// *****************************************************************************

/* ffffffff 00000010 */ EAGL::TARExtension::GetWidth() {}
/* ffffffff 00000014 */ EAGL::TARExtension::GetHeight() {}

// *****************************************************************************
// FILE -- D:/release/ps2/include/cmn/eagl/support/propparser.h
// *****************************************************************************

/* ffffffff 00000010 */ EAGLInternal::PropertyParser::MAXINT() {}
/* ffffffff 0000003c */ EAGLInternal::PropertyParser::SkipDelims() {}
/* ffffffff 00000088 */ EAGLInternal::PropertyParser::FindTokenEnd() {}

// *****************************************************************************
// FILE -- image.cpp
// *****************************************************************************

/* 00188670 000003bc */ EAGLInternal::Image::Init() {}
/* 00188a30 000001cc */ EAGLInternal::Image::BuildChains() {}
/* 00188c00 000001dc */ EAGLInternal::Image::TransferGstData() {}
/* 00188de0 000006b0 */ EAGLInternal::Image::TransferGst() {}
/* 00189490 000002e8 */ EAGLInternal::Image::Transfer() {}
/* 00189778 00000008 */ EAGLInternal::Image::Image() {}
/* 00189780 0000002c */ EAGLInternal::Image::~Image() {}
/* 001897b0 00000090 */ EAGLInternal::Image::Default() {}
/* 00189840 0000001c */ EAGLInternal::Image::Kill() {}
/* 00189860 00000044 */ EAGLInternal::Image::FreeChains() {}

// *****************************************************************************
// FILE -- state.cpp
// *****************************************************************************

/* 001898a8 00000e50 */ EAGLInternal::Constructors::RuntimeAllocGeoPrimStateConstructor() {}
/* 0018a6f8 000000bc */ EAGL::GeoPrimState::GeoPrimState() {}
/* 0018a7b8 000006ac */ EAGL::GeoPrimStateExtension::DumpState() {}
/* 0018ae68 00000034 */ static __static_initialization_and_destruction_0() {}
/* 0018aea0 00000020 */ EAGLInternal::Constructors::RuntimeAllocGeoPrimStateDestructor() {}
/* 0018aec0 00000050 */ EAGL::GeoPrimState::~GeoPrimState() {}
/* 0018af10 00000044 */ EAGL::GeoPrimState::GeoPrimState() {}
/* 0018af58 0000002c */ EAGL::GeoPrimState::operator=() {}
/* 0018af88 00000110 */ EAGL::GeoPrimStateExtension::GeoPrimStateExtension() {}
/* 0018b098 00000024 */ EAGL::GeoPrimStateExtension::~GeoPrimStateExtension() {}
/* 0018b0c0 00000040 */ EAGL::GeoPrimState::SetPrimitiveType() {}
/* 0018b100 0000002c */ EAGL::GeoPrimState::GetPrimitiveType() {}
/* 0018b130 0000004c */ EAGL::GeoPrimState::SetDepthTestMethod() {}
/* 0018b180 00000034 */ EAGL::GeoPrimState::GetDepthTestMethod() {}
/* 0018b1b8 00000008 */ EAGL::GeoPrimState::DumpState() {}
/* 0018b1c0 00000008 */ EAGL::GeoPrimStateExtension::DumpState() {}
/* 0018b1c8 00000020 */ global constructors keyed to state.cppnRaaaa() {}

// *****************************************************************************
// FILE -- D:/release/ps2/include/ps2/eagl/runtime/state.inl
// *****************************************************************************

/* ffffffff 0000001c */ EAGLInternal::GifState::SetFogging() {}
/* ffffffff 0000001c */ EAGLInternal::GifState::SetFST() {}
/* ffffffff 00000018 */ EAGLInternal::GifState::SetPrim() {}
/* ffffffff 0000001c */ EAGLInternal::GifState::SetShading() {}
/* ffffffff 0000001c */ EAGLInternal::GifState::SetTexture() {}
/* ffffffff 0000001c */ EAGLInternal::GifState::SetAlpha() {}
/* ffffffff 00000050 */ EAGLInternal::GifState::SetAlphaTestMethod() {}
/* ffffffff 00000050 */ EAGLInternal::GifState::SetDepthTestMethod() {}
/* ffffffff 0000009c */ EAGLInternal::GifState::SetDepthTest() {}
/* ffffffff 00000028 */ EAGLInternal::GifState::SetAlphaBlend() {}

// *****************************************************************************
// FILE -- D:/release/ps2/include/cmn/eagl/support/propparser.h
// *****************************************************************************

/* ffffffff 00000010 */ EAGLInternal::PropertyParser::MAXINT() {}
/* ffffffff 0000003c */ EAGLInternal::PropertyParser::SkipDelims() {}
/* ffffffff 00000088 */ EAGLInternal::PropertyParser::FindTokenEnd() {}

// *****************************************************************************
// FILE -- ../../../cmn/runtime/src/model.cpp
// *****************************************************************************

/* 0018b1e8 00000120 */ EAGL::Model::RemoveModel() {}
/* 0018b308 00000454 */ EAGL::Model::SetTextures() {}
/* 0018b760 00000280 */ EAGL::Model::GetTARList() {}
/* 0018b9e0 00000144 */ EAGL::Model::SetTexture() {}
/* 0018bb28 00000358 */ EAGL::Model::PS2SetOverlay() {}
/* 0018be80 000004d4 */ EAGL::Model::Draw() {}
/* 0018c358 000004c4 */ EAGL::Model::DrawInstances() {}
/* 0018c820 00000328 */ EAGL::Model::MorphModel() {}
/* 0018cb48 00000214 */ EAGL::Model::VerifyCurrentMorph() {}
/* 0018cd60 00000148 */ EAGL::DynamicModel::Preallocate() {}
/* 0018cea8 0000018c */ EAGL::DynamicModel::AddGeoPrim() {}
/* 0018d038 0000027c */ EAGL::DynamicModel::Draw() {}
/* 0018d2b8 0000009c */ EAGL::Model::VariableTable::find_end() {}
/* 0018d358 00000034 */ static __static_initialization_and_destruction_0() {}
/* 0018d390 0000002c */ EAGLInternal::ModelConstructor() {}
/* 0018d3c0 0000001c */ EAGLInternal::ModelDestructor() {}
/* 0018d3e0 000000dc */ EAGL::Model::GetChild() {}
/* 0018d4c0 00000024 */ EAGL::Model::AddFrozenModel() {}
/* 0018d4e8 00000024 */ EAGL::Model::AddHierarchyModel() {}
/* 0018d510 00000024 */ EAGL::Model::AddTransformedModel() {}
/* 0018d538 0000008c */ EAGL::Model::GetGeometry() {}
/* 0018d5c8 0000004c */ EAGL::Model::SetTexture() {}
/* 0018d618 00000008 */ EAGL::Model::Optimize() {}
/* 0018d620 00000044 */ EAGL::Model::Patch() {}
/* 0018d668 00000108 */ EAGL::Model::Model() {}
/* 0018d770 000000a8 */ EAGL::Model::Model() {}
/* 0018d818 0000004c */ EAGL::Model::~Model() {}
/* 0018d868 000000cc */ EAGL::Model::ClearMorphModel() {}
/* 0018d938 00000008 */ EAGL::Model::DumpState() {}
/* 0018d940 00000028 */ EAGL::Model::GetScale() {}
/* 0018d968 00000028 */ EAGL::Model::SetScale() {}
/* 0018d990 00000064 */ EAGL::DynamicModel::DynamicModel() {}
/* 0018d9f8 000000ec */ EAGL::DynamicModel::~DynamicModel() {}
/* 0018dae8 0000009c */ EAGL::DynamicModel::DrawNoTransform() {}
/* 0018db88 00000008 */ EAGL::DynamicModel::GetModelMatrix() {}
/* 0018db90 00000084 */ EAGL::DynamicModel::SetModelMatrix() {}
/* 0018dc18 000000a4 */ EAGL::DynamicModel::SetPrimitiveType() {}
/* 0018dcc0 000000a0 */ EAGL::DynamicModel::GetIndexFromName() {}
/* 0018dd60 000000c8 */ EAGL::DynamicModel::SetVar() {}
/* 0018de28 000000c8 */ EAGL::DynamicModel::SetVar() {}
/* 0018def0 000000c8 */ EAGL::DynamicModel::SetStream() {}
/* 0018dfb8 000000c8 */ EAGL::DynamicModel::SetStream() {}
/* 0018e080 000000a4 */ EAGL::DynamicModel::SetParamName() {}
/* 0018e128 00000098 */ EAGL::DynamicModel::Lock() {}
/* 0018e1c0 00000098 */ EAGL::DynamicModel::Unlock() {}
/* 0018e258 000000a4 */ EAGL::DynamicModel::SetNumVerts() {}
/* 0018e300 00000094 */ EAGL::Model::VariableTable::find_first() {}
/* 0018e398 00000010 */ EAGL::Model::Variable::IsModifiable() {}
/* 0018e3a8 0000009c */ EAGL::Model::Variable::CopyData() {}
/* 0018e448 00000080 */ static void EAGL::morphData<unsigned char>() {}
/* 0018e4c8 00000074 */ static void EAGL::morphData<short>() {}
/* 0018e540 00000064 */ static void EAGL::morphData<float>() {}
/* 0018e5a8 00000020 */ global constructors keyed to .._.._.._cmn_runtime_src_model.cpp5Raaaa() {}

// *****************************************************************************
// FILE -- D:/release/ps2/include/cmn/eagl/runtime/model.inl
// *****************************************************************************

/* ffffffff 0000001c */ EAGL::Model::ModelShape::TAR_iterator::TAR_iterator() {}
/* ffffffff 00000018 */ EAGL::Model::ModelShape_iterator::ModelShape_iterator() {}

// *****************************************************************************
// FILE -- platform_model.cpp
// *****************************************************************************

/* 0018e5c8 00000008 */ EAGL::Model::PlatformCall() {}
/* 0018e5d0 00000008 */ EAGL::Model::PreLight() {}
/* 0018e5d8 00000008 */ EAGL::Model::CheckIfSafeToDelete() {}

// *****************************************************************************
// FILE -- ../../../cmn/runtime/src/device_cmn.cpp
// *****************************************************************************

/* 0018e5e0 00000034 */ static __static_initialization_and_destruction_0() {}
/* 0018e618 00000068 */ EAGL::Device::NewRenderContext() {}
/* 0018e680 000000b4 */ EAGL::DeviceExtension::NewTextureRenderContext() {}
/* 0018e738 0000005c */ EAGL::Device::NewTextureRenderContext() {}
/* 0018e798 00000068 */ EAGL::Device::DeleteRenderContext() {}
/* 0018e800 000000c4 */ EAGL::DeviceExtension::DeleteTextureRenderContext() {}
/* 0018e8c8 00000034 */ EAGL::Device::DeleteTextureRenderContext() {}
/* 0018e900 00000014 */ EAGL::Device::AddRenderContext() {}
/* 0018e918 000000d4 */ EAGL::Device::RemoveRenderContext() {}
/* 0018e9f0 0000000c */ EAGL::Device::GetCurrentRenderContext() {}
/* 0018ea00 0000000c */ EAGL::Device::GetCurrentTextureRenderContext() {}
/* 0018ea10 0000000c */ EAGL::Device::GetCurrentContext() {}
/* 0018ea20 00000014 */ EAGLInternal::DevicePrivate::SetCurrentRenderContext() {}
/* 0018ea38 0000002c */ EAGLInternal::DevicePrivate::SetCurrentTextureRenderContext() {}
/* 0018ea68 0000000c */ EAGL::Device::SetNewOverride() {}
/* 0018ea78 0000000c */ EAGL::Device::SetDeleteOverride() {}
/* 0018ea88 0000000c */ EAGL::Device::Get() {}
/* 0018ea98 00000008 */ EAGL::Device::DumpCommonState() {}
/* 0018eaa0 00000020 */ global constructors keyed to .._.._.._cmn_runtime_src_device_cmn.cppOYaaaa() {}

// *****************************************************************************
// FILE -- dmamgr.cpp
// *****************************************************************************

/* 0018eac0 00000188 */ EAGLInternal::DmaMgr::Init() {}
/* 0018ec48 0000016c */ EAGLInternal::DmaMgr::AllocateAccumulators() {}
/* 0018edb8 00000280 */ EAGLInternal::DmaMgr::SwapAcc() {}
/* 0018f038 0000015c */ EAGLInternal::DmaMgr::WaitForDMACompletion() {}
/* 0018f198 00000124 */ EAGLInternal::DmaMgr::IntHandlerVIF() {}
/* 0018f2c0 000000fc */ EAGLInternal::DmaMgr::IntHandlerGIF() {}
/* 0018f3c0 000001d8 */ EAGLInternal::DmaMgr::IntHandlerDmacVIF() {}
/* 0018f598 000003d8 */ EAGLInternal::DmaMgr::WaitForEndGIF() {}
/* 0018f970 000003c4 */ EAGLInternal::DmaMgr::WaitForEndVIF() {}
/* 0018fd38 00000174 */ EAGLInternal::DmaMgr::VerifyChecksums() {}
/* 0018feb0 000000cc */ EAGLInternal::DMAPerf::TimeStamp() {}
/* 0018ff80 0000005c */ EAGLInternal::DmaMgr::DmaMgr() {}
/* 0018ffe0 000000c8 */ EAGLInternal::DmaMgr::~DmaMgr() {}
/* 001900a8 000000b8 */ EAGLInternal::DmaMgr::Kill() {}
/* 00190160 0000000c */ EAGLInternal::DmaMgr::SetNumbBuffers() {}
/* 00190170 00000098 */ EAGLInternal::DmaMgr::FreeAccumulators() {}
/* 00190208 000000a8 */ EAGLInternal::DmaMgr::ExecuteLists() {}
/* 001902b0 00000088 */ EAGLInternal::DmaMgr::InstallInterrupts() {}
/* 00190338 00000098 */ EAGLInternal::DmaMgr::RemoveInterrupts() {}
/* 001903d0 0000001c */ EAGLInternal::DmaMgr::ResetIntCounters() {}
/* 001903f0 000000c8 */ EAGLInternal::DmaMgr::CalcChecksums() {}
/* 001904b8 0000002c */ EAGLInternal::DMAPerf::SwapBuffers() {}
/* 001904e8 00000038 */ EAGLInternal::DMAPerf::InterTS() {}

// *****************************************************************************
// FILE -- paulvif.h
// *****************************************************************************

/* ffffffff 0000000c */ EAGLInternal::VifListAccumulator::Reset() {}
/* ffffffff 00000068 */ EAGLInternal::DMAAccumulator::Close() {}

// *****************************************************************************
// FILE -- dmamgr.h
// *****************************************************************************

/* ffffffff 00000024 */ EAGLInternal::DMAPerf::operator delete() {}

// *****************************************************************************
// FILE -- dma.cpp
// *****************************************************************************

/* 00190520 0000000c */ EAGL::SetVifDmaStartCallback() {}
/* 00190530 0000000c */ EAGL::SetGifDmaStartCallback() {}
/* 00190540 000000fc */ EAGLInternal::DMA_To_GIF() {}
/* 00190640 00000150 */ EAGLInternal::DMA_To_GIF_SourceChain() {}
/* 00190790 00000128 */ EAGLInternal::DMA_To_VIF1() {}
/* 001908b8 00000150 */ EAGLInternal::DMA_To_VIF1_SourceChain() {}
/* 00190a08 000000dc */ EAGLInternal::DMA_To_VIF0() {}
/* 00190ae8 000000dc */ EAGLInternal::DMA_From_VIF1() {}
/* 00190bc8 000000cc */ EAGLInternal::DMA_To_VIF0_SourceChain() {}
/* 00190c98 000000dc */ EAGLInternal::DMA_To_IPU() {}
/* 00190d78 000000cc */ EAGLInternal::DMA_To_IPU_SourceChain() {}
/* 00190e48 000000dc */ EAGLInternal::DMA_From_IPU() {}
/* 00190f28 000000dc */ EAGLInternal::DMA_From_SIF0() {}
/* 00191008 000000d4 */ EAGLInternal::DMA_To_SIF1() {}
/* 001910e0 000000cc */ EAGLInternal::DMA_To_SIF1_SourceChain() {}
/* 001911b0 000000d4 */ EAGLInternal::DMA_To_SIF2() {}
/* 00191288 000000d4 */ EAGLInternal::DMA_From_SIF2() {}
/* 00191360 000000ec */ EAGLInternal::DMA_From_ScratchPad() {}
/* 00191450 000000bc */ EAGLInternal::DMA_Reset() {}
/* 00191510 00000030 */ EAGLInternal::FlushDCache() {}

// *****************************************************************************
// FILE -- rendercontext.cpp
// *****************************************************************************

/* 00191540 00000204 */ EAGL::RenderContext::RenderContext() {}
/* 00191748 00000ad8 */ EAGLInternal::RenderContextPrivate::DoFilteredFlip() {}
/* 00192220 00000390 */ EAGL::RenderContext::BeginFrame() {}
/* 001925b0 00000154 */ EAGL::RenderContextExtension::RealEndFrame() {}
/* 00192708 000000d8 */ EAGL::RenderContext::EndFrame() {}
/* 001927e0 000004c8 */ EAGLInternal::RenderContextPrivate::DoWaitVSync() {}
/* 00192ca8 000002c8 */ EAGLInternal::RenderContextPrivate::SwapFrameRegs() {}
/* 00192f70 000001cc */ static Send8BitGSTLut() {}
/* 00193140 000001d4 */ static Send4BitGSTLut() {}
/* 00193318 0000037c */ EAGLInternal::RenderContextPrivate::RealSetupFrameBuffers() {}
/* 00193698 000007b0 */ EAGL::RenderContext::SetupFrameBuffers() {}
/* 00193e48 0000019c */ EAGL::RenderContextExtension::DisableZWrite() {}
/* 00193fe8 00000354 */ EAGL::RenderContextExtension::ReadRect() {}
/* 00194340 000003c0 */ EAGL::RenderContextExtension::ReadRectDuringFrame() {}
/* 00194700 00000168 */ EAGL::RenderContextExtension::GetTRC() {}
/* 00194868 0000015c */ EAGL::RenderContextExtension::SetAlphaCorrection() {}
/* 001949c8 000001ac */ EAGL::RenderContextExtension::SetRenderMask() {}
/* 00194b78 00000170 */ EAGL::RenderContextExtension::SetFogColour() {}
/* 00194ce8 00000310 */ EAGL::RenderContextExtension::SetDitherMatrix() {}
/* 00194ff8 0000014c */ EAGL::RenderContextExtension::SetPAMarkerData() {}
/* 00195148 00000710 */ EAGL::RenderContextExtension::MoveRG2BA() {}
/* 00195858 00000468 */ EAGL::RenderContextExtension::FogSprite() {}
/* 00195cc0 0000030c */ EAGL::RenderContextExtension::SetClutFog() {}
/* 00195fd0 000006d0 */ EAGL::RenderContextExtension::FogTexSprite() {}
/* 001966a0 00000158 */ EAGL::RenderContextExtension::SetDepthOfFieldClut() {}
/* 001967f8 000006f8 */ EAGL::RenderContextExtension::DoDepthOfField() {}
/* 00196ef0 000018a8 */ EAGL::RenderContextExtension::DoFlare() {}
/* 00198798 00000168 */ EAGLInternal::RenderContextPrivate::ClearVRAM() {}
/* 00198900 0000004c */ static __static_initialization_and_destruction_0() {}
/* 00198950 00000020 */ EAGL::RenderContextBase::RenderContextBase() {}
/* 00198970 00000078 */ EAGL::RenderContextBase::~RenderContextBase() {}
/* 001989e8 00000128 */ EAGL::RenderContext::~RenderContext() {}
/* 00198b10 000000cc */ EAGLInternal::RenderContextPrivate::DoKickPendingNopLoop() {}
/* 00198be0 0000000c */ EAGL::RenderContextExtension::SetDelayedDmaSpin() {}
/* 00198bf0 00000014 */ EAGL::RenderContextExtension::SetDTVOffsets() {}
/* 00198c08 00000018 */ EAGL::RenderContext::SetSize() {}
/* 00198c20 0000001c */ EAGL::RenderContext::GetSize() {}
/* 00198c40 00000008 */ EAGL::RenderContext::SetFrontBufferDepth() {}
/* 00198c48 00000008 */ EAGL::RenderContext::GetFrontBufferDepth() {}
/* 00198c50 00000008 */ EAGL::RenderContext::SetBackBufferDepth() {}
/* 00198c58 00000008 */ EAGL::RenderContext::GetBackBufferDepth() {}
/* 00198c60 00000008 */ EAGL::RenderContext::SetZBufferDepth() {}
/* 00198c68 00000008 */ EAGL::RenderContext::GetZBufferDepth() {}
/* 00198c70 00000008 */ EAGL::RenderContext::SetSyncToVBL() {}
/* 00198c78 00000008 */ EAGL::RenderContext::GetSyncToVBL() {}
/* 00198c80 0000000c */ EAGL::RenderContext::SetDitherEnable() {}
/* 00198c90 00000010 */ EAGL::RenderContext::GetDitherEnable() {}
/* 00198ca0 00000008 */ EAGL::RenderContext::DumpState() {}
/* 00198ca8 00000018 */ EAGL::RenderContextExtensionBase::RenderContextExtensionBase() {}
/* 00198cc0 00000030 */ EAGL::RenderContextExtensionBase::~RenderContextExtensionBase() {}
/* 00198cf0 00000030 */ EAGL::RenderContextExtension::RenderContextExtension() {}
/* 00198d20 00000030 */ EAGL::RenderContextExtension::~RenderContextExtension() {}
/* 00198d50 00000024 */ EAGLInternal::RenderContextPrivateBase::RenderContextPrivateBase() {}
/* 00198d78 00000030 */ EAGLInternal::RenderContextPrivateBase::~RenderContextPrivateBase() {}
/* 00198da8 00000100 */ EAGLInternal::RenderContextPrivate::RenderContextPrivate() {}
/* 00198ea8 00000030 */ EAGLInternal::RenderContextPrivate::~RenderContextPrivate() {}
/* 00198ed8 00000024 */ EAGLInternal::RenderContextPrivate::Screen::Screen() {}
/* 00198f00 0000004c */ EAGLInternal::RenderContextPrivate::FrameBuffer::FrameBuffer() {}
/* 00198f50 00000044 */ EAGLInternal::RenderContextPrivate::FlickerFilter::FlickerFilter() {}
/* 00198f98 00000040 */ EAGLInternal::RenderContextPrivate::Video::Video() {}
/* 00198fd8 0000006c */ EAGL::RenderContextExtension::TakeSnapShot() {}
/* 00199048 00000098 */ EAGL::RenderContextExtension::ReadRectAtEndOfFrame() {}
/* 001990e0 0000005c */ EAGL::RenderContextExtension::ReadRects() {}
/* 00199140 00000058 */ EAGL::RenderContextExtension::ReadImageDuringFrame() {}
/* 00199198 0000001c */ EAGL::RenderContextExtension::GetZBufferInfo() {}
/* 001991b8 00000028 */ EAGL::RenderContextExtension::GetBackBufferInfo() {}
/* 001991e0 0000000c */ EAGL::RenderContextExtension::GetFrontBuffer() {}
/* 001991f0 0000000c */ EAGL::RenderContextExtension::GetBackBuffer() {}
/* 00199200 0000000c */ EAGL::RenderContextExtension::GetZBuffer() {}
/* 00199210 00000014 */ EAGL::RenderContextExtension::SetFilterAlpha() {}
/* 00199228 0000002c */ EAGL::RenderContextExtension::SetFilterRGB() {}
/* 00199258 00000018 */ EAGL::RenderContextExtension::SetFilterOffset() {}
/* 00199270 0000000c */ EAGL::RenderContextExtension::SetDXPos() {}
/* 00199280 0000000c */ EAGL::RenderContextExtension::GetDXPos() {}
/* 00199290 0000000c */ EAGL::RenderContextExtension::SetDYPos() {}
/* 001992a0 0000000c */ EAGL::RenderContextExtension::GetDYPos() {}
/* 001992b0 0000000c */ EAGL::RenderContextExtension::SetEndFrameCallback() {}
/* 001992c0 0000000c */ EAGL::RenderContextExtension::SetTRCCalledEndFrame() {}
/* 001992d0 0000005c */ EAGLInternal::RenderContextPrivate::RemoveVBLHandler() {}
/* 00199330 0000006c */ EAGLInternal::RenderContextPrivate::InstallVBLHandler() {}
/* 001993a0 00000094 */ EAGLInternal::RenderContextPrivate::EAGLVBL_start() {}
/* 00199438 00000010 */ EAGLInternal::RenderContextPrivate::EAGLVBL_end() {}
/* 00199448 00000024 */ EAGL::RenderContextExtension::SetFrameRate() {}
/* 00199470 00000010 */ EAGL::RenderContextExtension::SetVBLKick() {}
/* 00199480 00000010 */ EAGL::RenderContextExtension::SetClearVRAMOnInit() {}
/* 00199490 0000000c */ EAGL::RenderContextExtension::GetClearVRAMOnInit() {}
/* 001994a0 00000020 */ EAGL::RenderContext::SetZWritesEnable() {}
/* 001994c0 00000010 */ EAGL::RenderContext::GetZWritesEnable() {}
/* 001994d0 00000064 */ EAGL::RenderContextExtension::SetVideoSignalType() {}
/* 00199538 000000c4 */ SetSpriteCoords() {}
/* 00199600 000000dc */ EAGL::RenderContextExtension::DoFog() {}
/* 001996e0 00000010 */ EAGL::RenderContextExtension::SetFogEnable() {}
/* 001996f0 00000014 */ EAGL::RenderContextExtension::GetFogEnable() {}
/* 00199708 00000010 */ EAGL::RenderContextExtension::SetFogTableMode() {}
/* 00199718 00000014 */ EAGL::RenderContextExtension::GetFogTableMode() {}
/* 00199730 00000010 */ EAGL::RenderContextExtension::SetFogStart() {}
/* 00199740 00000014 */ EAGL::RenderContextExtension::GetFogStart() {}
/* 00199758 00000010 */ EAGL::RenderContextExtension::SetFogEnd() {}
/* 00199768 00000014 */ EAGL::RenderContextExtension::GetFogEnd() {}
/* 00199780 00000010 */ EAGL::RenderContextExtension::SetFogDensity() {}
/* 00199790 00000014 */ EAGL::RenderContextExtension::GetFogDensity() {}
/* 001997a8 0000000c */ EAGL::RenderContextExtension::SetFilterBilinear() {}
/* 001997b8 00000038 */ EAGL::RenderContextExtension::SetFilterMode() {}
/* 001997f0 0000000c */ EAGL::RenderContextExtension::GetFogTAR() {}
/* 00199800 0000000c */ EAGL::RenderContextExtension::SetFilterStepSize() {}
/* 00199810 0000000c */ EAGL::RenderContextExtension::GetFilterStepSize() {}
/* 00199820 00000008 */ EAGL::RenderContextExtension::DumpState() {}
/* 00199828 00000020 */ EAGL::RenderContextExtension::SetHalfHeightBuffers() {}
/* 00199848 00000014 */ EAGL::RenderContextExtension::GetHalfHeightBuffers() {}
/* 00199860 00000080 */ EAGL::RenderContextExtension::WaitForDrawsToComplete() {}
/* 001998e0 00000020 */ global constructors keyed to rendercontext.cppzXaaaa() {}

// *****************************************************************************
// FILE -- D:/release/ps2/include/cmn/eagl/runtime/rendercontextbase_cmn.inl
// *****************************************************************************

/* ffffffff 00000008 */ EAGL::RenderContextBase::GetObjectType() {}

// *****************************************************************************
// FILE -- D:/release/ps2/include/cmn/eagl/runtime/rendercontext.h
// *****************************************************************************

/* ffffffff 00000024 */ EAGL::RenderContext::operator new() {}
/* ffffffff 00000024 */ EAGL::RenderContext::operator new() {}
/* ffffffff 00000024 */ EAGL::RenderContext::operator delete() {}
/* ffffffff 00000024 */ EAGL::RenderContext::operator new []() {}
/* ffffffff 00000024 */ EAGL::RenderContext::operator new []() {}
/* ffffffff 00000024 */ EAGL::RenderContext::operator delete []() {}
/* ffffffff 00000008 */ EAGL::RenderContext::operator new() {}
/* ffffffff 00000008 */ EAGL::RenderContext::operator delete() {}
/* ffffffff 00000008 */ EAGL::RenderContext::operator new []() {}
/* ffffffff 00000008 */ EAGL::RenderContext::operator delete []() {}

// *****************************************************************************
// FILE -- D:/release/ps2/include/cmn/eagl/runtime/rendercontext_cmn.inl
// *****************************************************************************

/* ffffffff 0000000c */ EAGL::RenderContext::GetCurrentFrameNumber() {}

// *****************************************************************************
// FILE -- D:/release/ps2/include/ps2/eagl/runtime/rendercontext.inl
// *****************************************************************************

/* ffffffff 00000008 */ EAGL::RenderContext::SetDepthBufferType() {}
/* ffffffff 00000008 */ EAGL::RenderContext::GetDepthBufferType() {}
/* ffffffff 00000010 */ EAGL::RenderContextExtension::ToggleSingleBuffered() {}

// *****************************************************************************
// FILE -- paulvif.h
// *****************************************************************************

/* ffffffff 00000068 */ EAGLInternal::DMAAccumulator::Close() {}

// *****************************************************************************
// FILE -- ../../../cmn/runtime/src/rendercontext_cmn.cpp
// *****************************************************************************

/* 00199900 0000004c */ static __static_initialization_and_destruction_0() {}
/* 00199950 00000090 */ EAGL::RenderContextBase::operator=() {}
/* 001999e0 00000058 */ EAGL::RenderContextBase::NewViewPort() {}
/* 00199a38 00000098 */ EAGL::RenderContextBase::DeleteViewPort() {}
/* 00199ad0 0000000c */ EAGL::RenderContextBase::GetCurrentViewPort() {}
/* 00199ae0 00000008 */ EAGLInternal::RenderContextPrivateBase::SetCurrentViewPort() {}
/* 00199ae8 00000008 */ EAGL::RenderContext::DumpCommonState() {}
/* 00199af0 00000020 */ global constructors keyed to .._.._.._cmn_runtime_src_rendercontext_cmn.cpp0Aaaaa() {}

// *****************************************************************************
// FILE -- texturerc.cpp
// *****************************************************************************

/* 00199b10 00000154 */ EAGL::TextureRenderContext::BeginFrame() {}
/* 00199c68 0000013c */ EAGL::TextureRenderContext::EndFrame() {}
/* 00199da8 000001f8 */ EAGL::TextureRenderContext::SetupFrameBuffers() {}
/* 00199fa0 00000198 */ EAGL::TextureRenderContextExtension::SetRenderMask() {}
/* 0019a138 0000015c */ EAGL::TextureRenderContextExtension::SetAlphaCorrection() {}
/* 0019a298 0000019c */ EAGL::TextureRenderContextExtension::DisableZWrite() {}
/* 0019a438 00000034 */ static __static_initialization_and_destruction_0() {}
/* 0019a470 0000001c */ EAGL::TextureRenderContext::GetSize() {}
/* 0019a490 00000008 */ EAGL::TextureRenderContext::GetBufferDepth() {}
/* 0019a498 00000008 */ EAGL::TextureRenderContext::GetZBufferDepth() {}
/* 0019a4a0 00000020 */ EAGL::TextureRenderContext::SetZWritesEnable() {}
/* 0019a4c0 00000010 */ EAGL::TextureRenderContext::GetZWritesEnable() {}
/* 0019a4d0 00000008 */ EAGL::TextureRenderContext::DumpState() {}
/* 0019a4d8 00000044 */ EAGL::TextureRenderContextExtension::TextureRenderContextExtension() {}
/* 0019a520 00000028 */ EAGL::TextureRenderContextExtension::~TextureRenderContextExtension() {}
/* 0019a548 00000070 */ EAGLInternal::TextureRenderContextPrivate::TextureRenderContextPrivate() {}
/* 0019a5b8 00000028 */ EAGLInternal::TextureRenderContextPrivate::~TextureRenderContextPrivate() {}
/* 0019a5e0 00000018 */ EAGLInternal::TextureRenderContextPrivate::Screen::Screen() {}
/* 0019a5f8 0000002c */ EAGLInternal::TextureRenderContextPrivate::FrameBuffer::FrameBuffer() {}
/* 0019a628 0000001c */ EAGL::TextureRenderContextExtension::GetZBufferInfo() {}
/* 0019a648 00000028 */ EAGL::TextureRenderContextExtension::GetBackBufferInfo() {}
/* 0019a670 00000008 */ EAGL::TextureRenderContextExtension::DumpState() {}
/* 0019a678 00000020 */ global constructors keyed to texturerc.cppoDaaaa() {}

// *****************************************************************************
// FILE -- D:/release/ps2/include/cmn/eagl/runtime/texturerc.h
// *****************************************************************************

/* ffffffff 00000024 */ EAGL::TextureRenderContext::operator new() {}
/* ffffffff 00000024 */ EAGL::TextureRenderContext::operator new() {}
/* ffffffff 00000024 */ EAGL::TextureRenderContext::operator delete() {}
/* ffffffff 00000024 */ EAGL::TextureRenderContext::operator new []() {}
/* ffffffff 00000024 */ EAGL::TextureRenderContext::operator new []() {}
/* ffffffff 00000024 */ EAGL::TextureRenderContext::operator delete []() {}
/* ffffffff 00000008 */ EAGL::TextureRenderContext::operator new() {}
/* ffffffff 00000008 */ EAGL::TextureRenderContext::operator delete() {}
/* ffffffff 00000008 */ EAGL::TextureRenderContext::operator new []() {}
/* ffffffff 00000008 */ EAGL::TextureRenderContext::operator delete []() {}

// *****************************************************************************
// FILE -- paulvif.h
// *****************************************************************************

/* ffffffff 00000068 */ EAGLInternal::DMAAccumulator::Close() {}

// *****************************************************************************
// FILE -- ../../../cmn/runtime/src/texturerc_cmn.cpp
// *****************************************************************************

/* 0019a698 00000034 */ static __static_initialization_and_destruction_0() {}
/* 0019a6d0 00000080 */ EAGL::TextureRenderContext::TextureRenderContext() {}
/* 0019a750 00000078 */ EAGL::TextureRenderContext::~TextureRenderContext() {}
/* 0019a7c8 00000008 */ EAGL::TextureRenderContext::DumpCommonState() {}
/* 0019a7d0 00000020 */ global constructors keyed to .._.._.._cmn_runtime_src_texturerc_cmn.cpplabaaa() {}

// *****************************************************************************
// FILE -- transform.cpp
// *****************************************************************************

/* 0019a7f0 00000270 */ EAGL::Transform::ExtractQuatTrans() {}
/* 0019aa60 000001c0 */ EAGL::Transform::TransformPoints() {}
/* 0019ac20 000001a0 */ EAGL::Transform::TransformPoints() {}
/* 0019adc0 00000258 */ EAGL::Transform::TransformPoints() {}
/* 0019b018 00000240 */ EAGL::Transform::TransformPoints() {}
/* 0019b258 00000208 */ EAGL::Transform::Invert() {}
/* 0019b460 00000164 */ EAGL::Transform::TransformPoint() {}
/* 0019b5c8 00000204 */ EAGL::Transform::TransformPoint() {}
/* 0019b7d0 0000001c */ EAGL::Transform::PostMult() {}
/* 0019b7f0 0000001c */ EAGL::Transform::PostMult() {}
/* 0019b810 00000044 */ EAGL::Transform::BuildScale() {}
/* 0019b858 0000004c */ EAGL::Transform::BuildTranslate() {}
/* 0019b8a8 0000004c */ EAGL::Transform::BuildIdentity() {}
/* 0019b8f8 0000004c */ EAGL::Transform::BuildZero() {}
/* 0019b948 00000078 */ EAGL::Transform::BuildRotTrans() {}
/* 0019b9c0 0000007c */ EAGL::Transform::BuildRotTrans() {}
/* 0019ba40 00000140 */ EAGL::Transform::BuildAimedTrans() {}
/* 0019bb80 000001a8 */ EAGL::Transform::BuildSRT() {}
/* 0019bd28 00000064 */ EAGL::Transform::ExtractRotTrans() {}
/* 0019bd90 00000084 */ EAGL::Transform::BuildMatrix() {}
/* 0019be18 000000f8 */ EAGL::Transform::BuildQuatTrans() {}
/* 0019bf10 00000120 */ EAGL::Transform::TransformPoints() {}
/* 0019c030 00000108 */ EAGL::Transform::TransformPoints() {}
/* 0019c138 00000100 */ EAGL::Transform::TransformPoints() {}
/* 0019c238 000000e8 */ EAGL::Transform::TransformPoints() {}
/* 0019c320 0000001c */ EAGL::Transform::Inverse() {}
/* 0019c340 00000044 */ EAGL::Transform::Inverse() {}
/* 0019c388 00000064 */ EAGL::Transform::Transpose() {}
/* 0019c3f0 00000084 */ EAGL::Transform::Transpose() {}
/* 0019c478 000000b4 */ EAGL::Transform::ElementMinor() {}
/* 0019c530 00000148 */ EAGL::Transform::Determinant() {}
/* 0019c678 000000f4 */ EAGL::Transform::AddWeightedSRT() {}
/* 0019c770 00000060 */ EAGL::Transform::AppendScale() {}
/* 0019c7d0 00000038 */ EAGL::Transform::AppendRotate() {}
/* 0019c808 00000068 */ EAGL::Transform::AppendTranslate() {}
/* 0019c870 0000009c */ EAGL::Transform::AppendRotTrans() {}
/* 0019c910 000000a0 */ EAGL::Transform::AppendRotTrans() {}
/* 0019c9b0 0000001c */ EAGL::Transform::AppendMatrix() {}
/* 0019c9d0 0000010c */ EAGL::Transform::AppendQuatTrans() {}
/* 0019cae0 00000150 */ EAGL::Transform::AppendAimedTrans() {}
/* 0019cc30 0000005c */ EAGL::Transform::PrependScale() {}
/* 0019cc90 00000034 */ EAGL::Transform::PrependRotate() {}
/* 0019ccc8 00000064 */ EAGL::Transform::PrependTranslate() {}
/* 0019cd30 00000094 */ EAGL::Transform::PrependRotTrans() {}
/* 0019cdc8 00000098 */ EAGL::Transform::PrependRotTrans() {}
/* 0019ce60 00000028 */ EAGL::Transform::PrependMatrix() {}
/* 0019ce88 00000108 */ EAGL::Transform::PrependQuatTrans() {}
/* 0019cf90 0000014c */ EAGL::Transform::PrependAimedTrans() {}
/* 0019d0e0 00000134 */ EAGL::Transform::TransformVector() {}
/* 0019d218 000000e8 */ EAGL::Transform::OrthoInverse() {}
/* 0019d300 000000a0 */ EAGL::Transform::OrthoInverse() {}
/* 0019d3a0 000000d8 */ EAGL::Transform::BuildSQT() {}
/* 0019d478 000000ac */ EAGL::Transform::BuildQT() {}
/* 0019d528 00000168 */ EAGL::Transform::ReplaceRotate() {}
/* 0019d690 0000001c */ MultMatrix() {}
/* 0019d6b0 00000024 */ EAGL::Transform::PreMult() {}
/* 0019d6d8 00000028 */ EAGL::Transform::PreMult() {}
/* 0019d700 000001c0 */ EAGL::Transform::BuildRotate() {}

// *****************************************************************************
// FILE -- viewport.cpp
// *****************************************************************************

/* 0019d8c0 00000384 */ EAGL::ViewPort::SetShape() {}
/* 0019dc48 00000300 */ EAGL::ViewPort::SetPerspective() {}
/* 0019df48 000001a4 */ EAGL::ViewPort::SetOrthographic() {}
/* 0019e0f0 00000230 */ EAGL::ViewPort::SetOrthographicScreenSpace() {}
/* 0019e320 00000598 */ EAGL::ViewPort::BeginView() {}
/* 0019e8b8 000006f8 */ EAGL::ViewPort::ClearViewPort() {}
/* 0019efb0 00000188 */ EAGL::ViewPort::IsSphereInView() {}
/* 0019f138 00000034 */ static __static_initialization_and_destruction_0() {}
/* 0019f170 00000044 */ EAGL::ViewPort::GetShape() {}
/* 0019f1b8 00000054 */ EAGLInternal::ViewPortPrivate::GetScale() {}
/* 0019f210 00000054 */ EAGLInternal::ViewPortPrivate::GetOffset() {}
/* 0019f268 000000a8 */ EAGL::ViewPort::SetViewMatrix() {}
/* 0019f310 00000030 */ EAGLInternal::ViewPortPrivate::ReBegin() {}
/* 0019f340 00000040 */ EAGL::ViewPort::EndView() {}
/* 0019f380 00000008 */ EAGL::ViewPort::SetGuardBandScale() {}
/* 0019f388 00000008 */ EAGL::ViewPort::GetGuardBandScale() {}
/* 0019f390 00000008 */ EAGL::ViewPort::DumpState() {}
/* 0019f398 00000008 */ EAGL::ViewPortExtension::ViewPortExtension() {}
/* 0019f3a0 0000000c */ EAGL::ViewPortExtension::ViewPortExtension() {}
/* 0019f3b0 00000024 */ EAGL::ViewPortExtension::~ViewPortExtension() {}
/* 0019f3d8 00000030 */ EAGLInternal::ViewPortPrivate::ViewPortPrivate() {}
/* 0019f408 00000008 */ EAGL::ViewPortExtension::DumpState() {}
/* 0019f410 00000020 */ global constructors keyed to viewport.cppkTaaaa() {}

// *****************************************************************************
// FILE -- paulvif.h
// *****************************************************************************

/* ffffffff 00000068 */ EAGLInternal::DMAAccumulator::Close() {}

// *****************************************************************************
// FILE -- ../../../cmn/runtime/src/viewport_cmn.cpp
// *****************************************************************************

/* 0019f430 00000034 */ static __static_initialization_and_destruction_0() {}
/* 0019f468 000001ec */ EAGL::ViewPort::ViewPort() {}
/* 0019f658 00000008 */ EAGL::ViewPort::SetEnableModelSphereCull() {}
/* 0019f660 00000008 */ EAGL::ViewPort::GetEnableModelSphereCull() {}
/* 0019f668 00000050 */ EAGL::ViewPort::~ViewPort() {}
/* 0019f6b8 00000024 */ EAGL::ViewPort::GetFrustum() {}
/* 0019f6e0 00000008 */ EAGL::ViewPort::GetProjectionType() {}
/* 0019f6e8 00000008 */ EAGL::ViewPort::SetBackgroundColour() {}
/* 0019f6f0 00000008 */ EAGL::ViewPort::GetBackgroundColour() {}
/* 0019f6f8 00000008 */ EAGL::ViewPort::GetViewMatrix() {}
/* 0019f700 00000008 */ EAGL::ViewPort::GetProjectionMatrix() {}
/* 0019f708 00000008 */ EAGL::ViewPort::GetViewProjectionMatrix() {}
/* 0019f710 00000008 */ EAGL::ViewPort::DumpCommonState() {}
/* 0019f718 00000020 */ global constructors keyed to .._.._.._cmn_runtime_src_viewport_cmn.cppf6aaaa() {}

// *****************************************************************************
// FILE -- vu0math.cpp
// *****************************************************************************

/* 0019f738 00000268 */ EAGL::VU0_m4toquat() {}
/* 0019f9a0 000004a8 */ EAGL::VU0_quatstoangvel() {}
/* 0019fe48 0000004c */ EAGL::VU0_MATRIX3x4_mult() {}
/* 0019fe98 00000068 */ EAGL::VU0_MATRIX3x4_multtrans() {}
/* 0019ff00 00000074 */ EAGL::VU0_MATRIX4_mult() {}
/* 0019ff78 000000a8 */ EAGL::VU0_MATRIX4_multb() {}
/* 001a0020 000000c8 */ EAGL::VU0_Sin() {}
/* 001a00e8 000000d4 */ EAGL::VU0_Cos() {}
/* 001a01c0 000001c4 */ EAGL::VU0_MATRIX4setxrot() {}
/* 001a0388 000001c4 */ EAGL::VU0_MATRIX4setyrot() {}
/* 001a0550 000001c4 */ EAGL::VU0_MATRIX4setzrot() {}
/* 001a0718 000000ac */ EAGL::VU0_fastqslerp() {}
/* 001a07c8 000000a4 */ EAGL::VU0_quattom4() {}
/* 001a0870 000000b8 */ EAGL::VU0_SQTquattom4() {}
/* 001a0928 0000003c */ EAGL::VU0_ExtractXAxis3FromQuat() {}
/* 001a0968 0000003c */ EAGL::VU0_ExtractXAxis4FromQuat() {}
/* 001a09a8 0000004c */ EAGL::VU0_ExtractYAxis3FromQuat() {}
/* 001a09f8 0000004c */ EAGL::VU0_ExtractYAxis4FromQuat() {}
/* 001a0a48 0000004c */ EAGL::VU0_ExtractZAxis3FromQuat() {}
/* 001a0a98 0000004c */ EAGL::VU0_ExtractZAxis4FromQuat() {}
/* 001a0ae8 00000008 */ EAGL::squ() {}
/* 001a0af0 0000000c */ EAGL::cub() {}
/* 001a0b00 000000c4 */ EAGL::VU0_UploadMicrocode() {}

// *****************************************************************************
// FILE -- ../../../cmn/runtime/src/profiler_cmn.cpp
// *****************************************************************************

/* 001a0bc8 000002bc */ EAGL::ProfilerRegion::DrawRegions() {}
/* 001a0e88 00000154 */ EAGL::ProfilerRegion::ProcessRegion() {}
/* 001a0fe0 00000744 */ EAGL::ProfilerRegion::DrawRegion() {}
/* 001a1728 00000130 */ EAGL::Profiler::ProcessProfile() {}
/* 001a1858 00000210 */ static __static_initialization_and_destruction_0() {}
/* 001a1a68 0000003c */ EAGL::ProfilerRegion::History::History() {}
/* 001a1aa8 00000090 */ EAGL::ProfilerRegion::History::SetHistoryValue() {}
/* 001a1b38 0000007c */ EAGL::ProfilerRegion::ProfilerRegion() {}
/* 001a1bb8 00000044 */ EAGL::ProfilerRegion::~ProfilerRegion() {}
/* 001a1c00 00000038 */ EAGL::ProfilerRegion::FreeMemory() {}
/* 001a1c38 0000003c */ EAGL::ProfilerRegion::ClearAllRegionsHighWaterMarks() {}
/* 001a1c78 0000008c */ EAGL::ProfilerRegion::ProcessRegions() {}
/* 001a1d08 0000009c */ EAGL::ProfilerRegion::ProcessInput() {}
/* 001a1da8 0000003c */ EAGL::ProfilerRegion::ResetAllCycleCounts() {}
/* 001a1de8 00000014 */ EAGL::ProfilerRegion::AddRegionToList() {}
/* 001a1e00 00000050 */ EAGL::ProfilerRegion::RemoveRegionFromList() {}
/* 001a1e50 0000003c */ EAGL::Profiler::OpenProfileSection() {}
/* 001a1e90 00000044 */ EAGL::Profiler::CloseProfileSection() {}
/* 001a1ed8 00000064 */ EAGL::Profiler::Profiler() {}
/* 001a1f40 00000078 */ EAGL::Profiler::Profiler() {}
/* 001a1fb8 000000b8 */ EAGL::Profiler::~Profiler() {}
/* 001a2070 00000020 */ global constructors keyed to .._.._.._cmn_runtime_src_profiler_cmn.cpp5zaaaa() {}
/* 001a2090 00000020 */ global destructors keyed to .._.._.._cmn_runtime_src_profiler_cmn.cpp0Zaaaa() {}

// *****************************************************************************
// FILE -- singledraw.cpp
// *****************************************************************************

/* 001a20b0 0000013c */ EAGL::DrawArray::SetGeoPrim() {}
/* 001a21f0 0000011c */ EAGL::DrawArray::SetUpGeoPrim() {}
/* 001a2310 000003ec */ EAGL::DrawGouraud::Init() {}
/* 001a2700 000001a4 */ EAGL::DrawGouraud::Begin() {}
/* 001a28a8 000001d0 */ EAGL::DrawGouraud::AddVertex() {}
/* 001a2a78 000003ec */ EAGL::DrawClipGouraud::Init() {}
/* 001a2e68 000001a4 */ EAGL::DrawClipGouraud::Begin() {}
/* 001a3010 000001d0 */ EAGL::DrawClipGouraud::AddVertex() {}
/* 001a31e0 000004e4 */ EAGL::DrawTextured::Init() {}
/* 001a36c8 000001a4 */ EAGL::DrawTextured::Begin() {}
/* 001a3870 00000224 */ EAGL::DrawTextured::AddVertex() {}
/* 001a3a98 00000158 */ EAGL::DrawTextured::SetTexture() {}
/* 001a3bf0 000004e4 */ EAGL::DrawClipTextured::Init() {}
/* 001a40d8 000001a4 */ EAGL::DrawClipTextured::Begin() {}
/* 001a4280 00000224 */ EAGL::DrawClipTextured::AddVertex() {}
/* 001a44a8 00000158 */ EAGL::DrawClipTextured::SetTexture() {}
/* 001a4600 00000060 */ EAGL::DrawArray::ParamBufAlloc() {}
/* 001a4660 00000058 */ EAGL::DrawArray::ParamBufFree() {}
/* 001a46b8 0000008c */ EAGL::DrawArray::DrawArray() {}
/* 001a4748 00000088 */ EAGL::DrawArray::~DrawArray() {}
/* 001a47d0 000000a4 */ EAGL::DrawArray::SetPrimitiveType() {}
/* 001a4878 00000090 */ EAGL::DrawArray::GetIndexFromName() {}
/* 001a4908 0000001c */ EAGL::DrawArray::GetNameFromIndex() {}
/* 001a4928 00000098 */ EAGL::DrawArray::SetParamName() {}
/* 001a49c0 000000c8 */ EAGL::DrawArray::SetVar() {}
/* 001a4a88 000000d4 */ EAGL::DrawArray::SetVar() {}
/* 001a4b60 00000014 */ EAGL::DrawArray::Lock() {}
/* 001a4b78 0000000c */ EAGL::DrawArray::Unlock() {}
/* 001a4b88 000000c4 */ EAGL::DrawArray::Draw() {}
/* 001a4c50 0000000c */ EAGL::DrawArray::SetNumVerts() {}
/* 001a4c60 00000020 */ EAGL::DrawArray::SetLocalMatrix() {}
/* 001a4c80 000000ec */ EAGL::DrawGouraud::DrawGouraud() {}
/* 001a4d70 000000d4 */ EAGL::DrawGouraud::~DrawGouraud() {}
/* 001a4e48 000000d4 */ EAGL::DrawGouraud::InternalFlush() {}
/* 001a4f20 000000ec */ EAGL::DrawGouraud::End() {}
/* 001a5010 00000040 */ EAGL::DrawGouraud::SetState() {}
/* 001a5050 00000008 */ EAGL::DrawGouraud::GetState() {}
/* 001a5058 0000001c */ EAGL::DrawGouraud::SetModelMatrix() {}
/* 001a5078 000000ec */ EAGL::DrawClipGouraud::DrawClipGouraud() {}
/* 001a5168 000000d4 */ EAGL::DrawClipGouraud::~DrawClipGouraud() {}
/* 001a5240 000000d4 */ EAGL::DrawClipGouraud::InternalFlush() {}
/* 001a5318 000000ec */ EAGL::DrawClipGouraud::End() {}
/* 001a5408 00000040 */ EAGL::DrawClipGouraud::SetState() {}
/* 001a5448 00000008 */ EAGL::DrawClipGouraud::GetState() {}
/* 001a5450 0000001c */ EAGL::DrawClipGouraud::SetModelMatrix() {}
/* 001a5470 000000f0 */ EAGL::DrawTextured::DrawTextured() {}
/* 001a5560 000000ec */ EAGL::DrawTextured::~DrawTextured() {}
/* 001a5650 000000d4 */ EAGL::DrawTextured::InternalFlush() {}
/* 001a5728 000000ec */ EAGL::DrawTextured::End() {}
/* 001a5818 00000040 */ EAGL::DrawTextured::SetState() {}
/* 001a5858 00000008 */ EAGL::DrawTextured::GetState() {}
/* 001a5860 0000001c */ EAGL::DrawTextured::SetModelMatrix() {}
/* 001a5880 000000f0 */ EAGL::DrawClipTextured::DrawClipTextured() {}
/* 001a5970 000000ec */ EAGL::DrawClipTextured::~DrawClipTextured() {}
/* 001a5a60 000000d4 */ EAGL::DrawClipTextured::InternalFlush() {}
/* 001a5b38 000000ec */ EAGL::DrawClipTextured::End() {}
/* 001a5c28 00000040 */ EAGL::DrawClipTextured::SetState() {}
/* 001a5c68 00000008 */ EAGL::DrawClipTextured::GetState() {}
/* 001a5c70 0000001c */ EAGL::DrawClipTextured::SetModelMatrix() {}

// *****************************************************************************
// FILE -- ../../../cmn/runtime/src/symbolinit.cpp
// *****************************************************************************

/* 001a5c90 0000008c */ EAGL::SymbolInit() {}

// *****************************************************************************
// FILE -- dmadebug.cpp
// *****************************************************************************

/* 001a5d20 00000440 */ EAGLInternal::VIFSim() {}
/* 001a6160 00000708 */ EAGLInternal::DumpDMA() {}
/* 001a6868 00000bbc */ EAGLInternal::dump_registers() {}
/* 001a7428 00000024 */ EAGLInternal::EaglVifDmaCallback() {}
/* 001a7450 00000030 */ EAGLInternal::EaglGifDmaCallback() {}

// *****************************************************************************
// FILE -- texallocator.cpp
// *****************************************************************************

/* 001a7480 0000023c */ EAGLInternal::TexAlloc::Use() {}
/* 001a76c0 000003c0 */ EAGLInternal::TexAlloc::Dump() {}
/* 001a7a80 00000548 */ EAGLInternal::TexAlloc::Dump() {}
/* 001a7fc8 00000034 */ EAGLInternal::TexAlloc::TexAlloc() {}
/* 001a8000 00000064 */ EAGLInternal::TexAlloc::~TexAlloc() {}
/* 001a8068 00000064 */ EAGLInternal::TexAlloc::Init() {}
/* 001a80d0 00000044 */ EAGLInternal::TexAlloc::ResetBase() {}
/* 001a8118 0000005c */ EAGLInternal::TexAlloc::MakeLockMark() {}
/* 001a8178 0000001c */ EAGLInternal::TexAlloc::Lock() {}
/* 001a8198 000000a8 */ EAGLInternal::TexAlloc::UnlockToMark() {}
/* 001a8240 00000078 */ EAGLInternal::TexAlloc::UnlockAll() {}
/* 001a82b8 00000078 */ EAGLInternal::TexAlloc::Flush() {}

// *****************************************************************************
// FILE -- fonteagl.cpp
// *****************************************************************************

/* 001a8330 00000344 */ EAGL::EAGLFont::FONTEAGL_draw() {}
/* 001a8678 000004c0 */ EAGL::EAGLFont::FONTEAGL_startdraw() {}
/* 001a8b38 00000168 */ EAGL::EAGLFont::FONTEAGL_enddraw() {}
/* 001a8ca0 000003b0 */ EAGL::EAGLFont::FONTEAGL_drawarray() {}
/* 001a9050 000003a4 */ EAGL::EAGLFont::FONTEAGL_build() {}
/* 001a93f8 00000438 */ EAGL::EAGLFont::FONTEAGL_drawcached() {}
/* 001a9830 00000068 */ EAGL::EAGLFont::FONTEAGL_createfont() {}
/* 001a9898 00000048 */ EAGL::EAGLFont::FONTEAGL_destroyfont() {}
/* 001a98e0 0000000c */ EAGL::EAGLFont::FONTEAGL_getsize() {}
/* 001a98f0 00000008 */ EAGL::EAGLFont::FONTEAGL_setstate() {}
/* 001a98f8 00000008 */ EAGL::EAGLFont::FONTEAGL_setstatef() {}

// *****************************************************************************
// FILE -- gst.cpp
// *****************************************************************************

/* 001a9900 000000c8 */ EAGLInternal::GetGstInfo() {}
/* 001a99c8 00000054 */ EAGLInternal::Build4BitDetailLut() {}
/* 001a9a20 00000070 */ EAGLInternal::Build8BitDetailLut() {}

// *****************************************************************************
// FILE -- ../../../cmn/support/src/base.cpp
// *****************************************************************************

/* 001a9a90 0000001c */ static EAGLInternal::DefaultMalloc() {}
/* 001a9ab0 0000001c */ static EAGLInternal::DefaultFree() {}

// *****************************************************************************
// FILE -- ../../../cmn/support/src/conspool.cpp
// *****************************************************************************

/* 001a9ad0 00000078 */ EAGL::ConstructorPool::~ConstructorPool() {}
/* 001a9b48 0000003c */ EAGL::ConstructorPool::RemoveType() {}
/* 001a9b88 0000004c */ EAGL::ConstructorPool::AddType() {}
/* 001a9bd8 00000024 */ EAGL::ConstructorPool::FindConstructor() {}
/* 001a9c00 00000028 */ EAGL::ConstructorPool::FindDestructor() {}
/* 001a9c28 00000078 */ EAGL::RuntimeAllocConstructorPool::~RuntimeAllocConstructorPool() {}
/* 001a9ca0 0000003c */ EAGL::RuntimeAllocConstructorPool::RemoveType() {}
/* 001a9ce0 0000004c */ EAGL::RuntimeAllocConstructorPool::AddType() {}
/* 001a9d30 00000024 */ EAGL::RuntimeAllocConstructorPool::FindConstructor() {}
/* 001a9d58 00000028 */ EAGL::RuntimeAllocConstructorPool::FindDestructor() {}
/* 001a9d80 0000002c */ EAGL::RuntimeAllocConstructorPool::Empty() {}
/* 001a9db0 0000002c */ EAGL::RuntimeAllocConstructorPool::FreeMemory() {}
/* 001a9de0 0000002c */ EAGL::ConstructorPool::Empty() {}
/* 001a9e10 0000002c */ EAGL::ConstructorPool::FreeMemory() {}

// *****************************************************************************
// FILE -- ../../../cmn/support/src/printmessage.cpp
// *****************************************************************************

/* 001a9e40 00000074 */ EAGL::Print() {}
/* 001a9eb8 00000084 */ EAGL::PrintMessage() {}
/* 001a9f40 0000000c */ EAGL::SetPrintHandler() {}
/* 001a9f50 0000000c */ EAGL::GetPrintHandler() {}
/* 001a9f60 00000048 */ EAGL::PrintVaList() {}
/* 001a9fa8 0000000c */ EAGL::SetPrintVerboseness() {}

// *****************************************************************************
// FILE -- ../../../cmn/support/src/cmnstateprop.cpp
// *****************************************************************************

/* 001a9fb8 00000518 */ EAGLInternal::StringToCommonStateEnum() {}
/* 001aa4d0 000004d4 */ EAGLInternal::HandleDefaultStateProp() {}

// *****************************************************************************
// FILE -- ../../../cmn/support/src/dlopen.cpp
// *****************************************************************************

/* 001aa9a8 00000168 */ EAGL::DynamicLoader::RunConstructors() {}
/* 001aab10 000005f8 */ EAGL::DynamicLoader::Resolve() {}
/* 001ab108 00000464 */ EAGL::DynamicLoader::ReResolve() {}
/* 001ab570 000006e8 */ EAGL::DynamicLoader::Initialize() {}
/* 001abc58 0000024c */ EAGL::DynamicLoader::GetAddr() {}
/* 001abea8 00000144 */ EAGL::DynamicLoader::GetIndex() {}
/* 001abff0 0000013c */ EAGL::DynamicLoader::GetSymbol() {}
/* 001ac130 0000018c */ static dlsym() {}
/* 001ac2c0 0000009c */ static __static_initialization_and_destruction_0() {}
/* 001ac360 0000006c */ EAGL::DynamicLoader::DynamicLoader() {}
/* 001ac3d0 00000080 */ EAGL::DynamicLoader::DynamicLoader() {}
/* 001ac450 00000008 */ EAGL::DynamicLoader::DoVersionCheck() {}
/* 001ac458 00000070 */ EAGL::DynamicLoader::~DynamicLoader() {}
/* 001ac4c8 000000bc */ EAGL::DynamicLoader::Release() {}
/* 001ac588 000000dc */ EAGL::DynamicLoader::RunDestructors() {}
/* 001ac668 00000018 */ EAGL::DynamicLoader::GetCount() {}
/* 001ac680 00000008 */ EAGL::DynamicLoader::GetElfData() {}
/* 001ac688 000000c8 */ EAGL::DynamicLoader::GetNextSymbol() {}
/* 001ac750 000000f0 */ EAGL::DynamicLoader::GetNextAddr() {}
/* 001ac840 00000028 */ EAGL::DynamicLoader::RegisterVar() {}
/* 001ac868 00000024 */ EAGL::DynamicLoader::UnRegisterVar() {}
/* 001ac890 00000028 */ EAGL::DynamicLoader::GetRegisteredVar() {}
/* 001ac8b8 000000e4 */ EAGL::DynamicLoader::AddPatchAddress() {}
/* 001ac9a0 00000020 */ global constructors keyed to .._.._.._cmn_support_src_dlopen.cpprOaaaa() {}
/* 001ac9c0 00000020 */ global destructors keyed to .._.._.._cmn_support_src_dlopen.cppwtbaaa() {}

// *****************************************************************************
// FILE -- ../../../cmn/support/src/sympool.cpp
// *****************************************************************************

/* 001ac9e0 000001f8 */ EAGL::SymbolPool::Insert() {}
/* 001acbd8 00000124 */ EAGL::SymbolPool::RemoveSymbol() {}
/* 001acd00 0000011c */ EAGL::SymbolPool::Search() {}
/* 001ace20 0000000c */ EAGL::SetSymbolPoolDefaultSize() {}
/* 001ace30 00000020 */ EAGL::SymbolPool::SymbolPool() {}
/* 001ace50 00000044 */ EAGL::SymbolPool::FreeMemory() {}
/* 001ace98 0000006c */ EAGL::SymbolPool::~SymbolPool() {}
/* 001acf08 00000048 */ EAGL::SymbolPool::HashFunction() {}
/* 001acf50 00000094 */ EAGL::SymbolPool::AddSymbol() {}
/* 001acfe8 00000064 */ EAGL::SymbolPool::AddFunction() {}
/* 001ad050 00000074 */ EAGL::SymbolPool::RemoveFunction() {}
/* 001ad0c8 000000e4 */ EAGL::SymbolPool::Empty() {}
/* 001ad1b0 0000000c */ EAGL::SymbolPool::GenerateCRC() {}
/* 001ad1c0 00000008 */ EAGL::SymbolPool::IsSymbolTableValid() {}
/* 001ad1c8 00000040 */ EAGL::SymbolPool::GetFirstPosition() {}
/* 001ad208 0000003c */ EAGL::SymbolPool::GetNextPosition() {}
/* 001ad248 00000010 */ EAGL::SymbolPool::IsPositionAtEnd() {}
/* 001ad258 00000018 */ EAGL::SymbolPool::GetSymbolName() {}
/* 001ad270 00000018 */ EAGL::SymbolPool::GetSymbolAddr() {}

// *****************************************************************************
// FILE -- ../../../cmn/support/src/bitfield.cpp
// *****************************************************************************

/* 001ad288 00000048 */ EAGLInternal::BitField::Create() {}
/* 001ad2d0 00000040 */ EAGLInternal::BitField::Destroy() {}
/* 001ad310 00000008 */ EAGLInternal::BitField::GetNumBits() {}
/* 001ad318 0000005c */ EAGLInternal::BitField::SetAllBits() {}
/* 001ad378 0000002c */ EAGLInternal::BitField::GetBit() {}
/* 001ad3a8 00000050 */ EAGLInternal::BitField::SetBit() {}
/* 001ad3f8 0000001c */ EAGLInternal::BitField::GetMemoryImageSize() {}
/* 001ad418 00000020 */ EAGLInternal::BitField::GetMemoryImageSize() {}

// *****************************************************************************
// FILE -- rendermethod.cpp
// *****************************************************************************

/* 001ad438 00002514 */ EAGL::RenderMethod::Draw() {}
/* 001af950 00000870 */ EAGL::RenderMethod::Compile() {}
/* 001b01c0 000000b0 */ EAGLInternal::ModelSetScale() {}
/* 001b0270 00000038 */ EAGL::RenderMethod::Optimize() {}
/* 001b02a8 000000a0 */ EAGL::RenderMethod::Release() {}

// *****************************************************************************
// FILE -- allocator.h
// *****************************************************************************

/* ffffffff 00000074 */ EAGLInternal::VUmuAlloc::Flush() {}

// *****************************************************************************
// FILE -- src/mem/cmn/memclass.cpp
// *****************************************************************************

/* 001b1830 00000028 */ MEMCLASS_init() {}
/* 001b1858 0000029c */ MEMCLASS_create() {}
/* 001b1af8 00000070 */ MEMCLASS_remove() {}
/* 001b1b68 00000078 */ MEMCLASS_purge() {}
/* 001b1be0 00000040 */ MEMCLASS_getfree() {}
/* 001b1c20 00000048 */ MEMCLASS_setAllocFreeCallbacks() {}

// *****************************************************************************
// FILE -- src/mem/cmn/memunused.cpp
// *****************************************************************************

/* 001b1c68 00000098 */ MEM_largestunused() {}
/* 001b1d00 00000068 */ MEM_totalunused() {}

// *****************************************************************************
// FILE -- src/mem/cmn/memalloc.cpp
// *****************************************************************************

/* 001b1d68 00000020 */ MEM_tailsize() {}
/* 001b1d88 000005f4 */ static MEM_allocaligna() {}
/* 001b2380 0000001c */ MEM_allocalign() {}
/* 001b23a0 00000028 */ MEM_alloc() {}
/* 001b23c8 00000028 */ MEM_allocz() {}
/* 001b23f0 00000030 */ MEM_allocsize() {}
/* 001b2420 00000030 */ MEM_allocsizez() {}

// *****************************************************************************
// FILE -- src/mem/cmn/memfree.cpp
// *****************************************************************************

/* 001b2450 000002c4 */ MEM_free() {}

// *****************************************************************************
// FILE -- src/mem/cmn/meminit.cpp
// *****************************************************************************

/* 001b2718 00000070 */ MEM_initsize() {}
/* 001b2788 0000001c */ MEM_init() {}

// *****************************************************************************
// FILE -- src/mem/cmn/meminitadr.cpp
// *****************************************************************************

/* 001b27a8 00000048 */ MEM_initadr() {}

// *****************************************************************************
// FILE -- src/mem/cmn/memrestore.cpp
// *****************************************************************************

/* 001b27f0 000000d4 */ MEM_restore() {}

// *****************************************************************************
// FILE -- src/mem/cmn/memblock.cpp
// *****************************************************************************

/* 001b28c8 00000020 */ MEM_initblock() {}

// *****************************************************************************
// FILE -- src/mem/cmn/memstd.cpp
// *****************************************************************************

/* 001b28e8 00000008 */ MEM_size() {}
/* 001b28f0 00000008 */ MEM_type() {}
/* 001b28f8 00000008 */ MEM_name() {}
/* 001b2900 0000002c */ MEM_comment() {}
/* 001b2930 00000030 */ MEM_setcomment() {}

// *****************************************************************************
// FILE -- src/mem/cmn/memprint.cpp
// *****************************************************************************

/* 001b2960 0000005c */ static MEMPRINT_string() {}
/* 001b29c0 00000028 */ MEM_printclass() {}
/* 001b29e8 00000024 */ MEM_print() {}

// *****************************************************************************
// FILE -- src/mem/cmn/memprintf.cpp
// *****************************************************************************

/* 001b2a10 0000021c */ MEM_printclassf() {}

// *****************************************************************************
// FILE -- src/mem/cmn/memvalid.cpp
// *****************************************************************************

/* 001b2c30 0000006c */ static checkfree() {}
/* 001b2ca0 00000100 */ checksentinel() {}
/* 001b2da0 000000c4 */ static checknextprevptrs() {}
/* 001b2e68 0000009c */ static checkfreelist() {}
/* 001b2f08 000000cc */ static validatememclass() {}
/* 001b2fd8 0000006c */ MEM_validate() {}

// *****************************************************************************
// FILE -- src/codec/cmn/unpack.cpp
// *****************************************************************************

/* 001b3048 000000b8 */ unpackz() {}
/* 001b3100 0000001c */ unpack() {}
/* 001b3120 00000198 */ unpacksizez() {}
/* 001b32b8 0000001c */ unpacksize() {}

// *****************************************************************************
// FILE -- src/codec/cmn/refdecode.cpp
// *****************************************************************************

/* 001b32d8 00000330 */ REF_decode() {}

// *****************************************************************************
// FILE -- src/input/ps2/pad.cpp
// *****************************************************************************

/* 001b3608 00000030 */ PAD_getconfigopts() {}
/* 001b3638 00000030 */ PAD_setconfigopts() {}
/* 001b3668 00000154 */ PAD_init() {}
/* 001b37c0 000000d0 */ PAD_restore() {}
/* 001b3890 00000014 */ PAD_getdataptr() {}
/* 001b38a8 00000018 */ PAD_getpadtype() {}
/* 001b38c0 0000004c */ PAD_active() {}
/* 001b3910 00000074 */ PAD_scan() {}
/* 001b3988 00000044 */ PAD_state() {}
/* 001b39d0 0000004c */ PAD_eventpress() {}
/* 001b3a20 00000048 */ PAD_getstickinfo() {}
/* 001b3a68 00000200 */ PAD_getbuttonpressure() {}
/* 001b3c68 00000040 */ PAD_getpressureinfo() {}
/* 001b3ca8 00000094 */ PAD_setmode() {}
/* 001b3d40 00000070 */ PAD_setactuator() {}
/* 001b3db0 00000034 */ PAD_getactuator() {}
/* 001b3de8 000003e0 */ static iUpdateControllerBuf() {}
/* 001b41c8 00000090 */ PAD_update() {}
/* 001b4258 00000074 */ PAD_isstable() {}

// *****************************************************************************
// FILE -- src/file/ps2/filesys.cpp
// *****************************************************************************

/* 001b42d0 000000ac */ FILESYS_init() {}
/* 001b4380 0000010c */ FILESYS_initadr() {}
/* 001b4490 00000048 */ FILESYS_overhead() {}
/* 001b44d8 000000ac */ FILESYS_restore() {}
/* 001b4588 00000080 */ FILESYS_opstatus() {}
/* 001b4608 0000002c */ FILESYS_operror() {}
/* 001b4638 0000002c */ FILESYS_opuserdata() {}
/* 001b4668 00000050 */ FILESYS_callbackop() {}
/* 001b46b8 00000184 */ FILESYS_priorityop() {}
/* 001b4840 00000090 */ FILESYS_cancelop() {}
/* 001b48d0 00000138 */ FILESYS_waitop() {}
/* 001b4a08 00000124 */ FILESYS_completeop() {}
/* 001b4b30 000000b0 */ FILESYS_exists() {}
/* 001b4be0 000000b8 */ FILESYS_open() {}
/* 001b4c98 000000a8 */ FILESYS_close() {}
/* 001b4d40 000000d4 */ FILESYS_read() {}
/* 001b4e18 000000c0 */ FILESYS_write() {}
/* 001b4ed8 00000098 */ FILESYS_size() {}
/* 001b4f70 0000006c */ FILESYS_null() {}
/* 001b4fe0 00000124 */ iFILE_addbigreadcallback() {}
/* 001b5108 000000ac */ iFILE_addbigopencallback() {}
/* 001b51b8 00000140 */ FILESYS_addbig() {}
/* 001b52f8 0000002c */ iFILE_delbigclosecallback() {}
/* 001b5328 000001bc */ FILESYS_delbig() {}
/* 001b54e8 00000060 */ FILESYS_atomic() {}
/* 001b5548 00000024 */ FILESYS_setminpriority() {}
/* 001b5570 00000108 */ FILESYS_setcurrentpath() {}
/* 001b5678 00000024 */ FILESYS_getcurrentpath() {}
/* 001b56a0 00000474 */ static iFILESYS_ExecCommand() {}
/* 001b5b18 000000a4 */ iFILESYS_CommandCompleteCallback() {}
/* 001b5bc0 00000008 */ FILESYS_seteventcb() {}
/* 001b5bc8 00000198 */ FILESYS_bypassqueuefileinfo() {}
/* 001b5d60 00000008 */ static iFILE_perror() {}
/* 001b5d68 00000140 */ static reserveop() {}
/* 001b5ea8 00000050 */ static freeop() {}
/* 001b5ef8 000000bc */ static reservehandle() {}
/* 001b5fb8 00000050 */ static freehandle() {}
/* 001b6008 00000014 */ FILESYS_setmemcallbacks() {}

// *****************************************************************************
// FILE -- src/file/cmn/filesysopts.cpp
// *****************************************************************************

/* 001b6020 0000008c */ FILESYS_getopts() {}
/* 001b60b0 00000090 */ FILESYS_setopts() {}

// *****************************************************************************
// FILE -- src/file/cmn/syncfile.cpp
// *****************************************************************************

/* 001b6140 00000108 */ static synccallback() {}
/* 001b6248 0000009c */ static syncblockio() {}
/* 001b62e8 00000070 */ FILESYS_opensync() {}
/* 001b6358 00000020 */ FILESYS_readsync() {}
/* 001b6378 00000020 */ FILESYS_writesync() {}
/* 001b6398 00000054 */ FILESYS_closesync() {}
/* 001b63f0 00000054 */ FILESYS_sizesync() {}
/* 001b6448 00000070 */ FILESYS_addbigsync() {}
/* 001b64b8 00000054 */ FILESYS_delbigsync() {}
/* 001b6510 00000054 */ FILESYS_existssync() {}

// *****************************************************************************
// FILE -- src/file/cmn/hlafile.cpp
// *****************************************************************************

/* 001b6568 00000068 */ static queueadd() {}
/* 001b65d0 00000064 */ static queuefetch() {}
/* 001b6638 00000030 */ static newrequestid() {}
/* 001b6668 0000004c */ static locaterequest() {}
/* 001b66b8 00000068 */ static releaserequest() {}
/* 001b6720 00000090 */ static finishrequest() {}
/* 001b67b0 00000034 */ static loadfileclosecallback() {}
/* 001b67e8 000000ec */ static loadfilereadcallback() {}
/* 001b68d8 000000dc */ static loadfilesizecallback() {}
/* 001b69b8 00000100 */ static loadfileopencallback() {}
/* 001b6ab8 000000c4 */ static readfilereadcallback() {}
/* 001b6b80 000000e4 */ ASYNCFILE_init() {}
/* 001b6c68 0000011c */ ASYNCFILE_restore() {}
/* 001b6d88 000000b4 */ ASYNCFILE_load() {}
/* 001b6e40 000000bc */ ASYNCFILE_loadat() {}
/* 001b6f00 000000dc */ ASYNCFILE_read() {}
/* 001b6fe0 00000094 */ ASYNCFILE_setcallback() {}
/* 001b7078 00000058 */ ASYNCFILE_getstatus() {}
/* 001b70d0 00000118 */ ASYNCFILE_release() {}
/* 001b71e8 000000dc */ ASYNCFILE_cancel() {}

// *****************************************************************************
// FILE -- src/file/cmn/hlsfile.cpp
// *****************************************************************************

/* 001b72c8 00000024 */ FILE_exists() {}
/* 001b72f0 0000006c */ static filesizeatom() {}
/* 001b7360 0000003c */ FILE_size() {}
/* 001b73a0 00000038 */ FILE_sizez() {}
/* 001b73d8 000000b8 */ static loadfileatom() {}
/* 001b7490 00000040 */ FILE_load() {}
/* 001b74d0 0000003c */ FILE_loadz() {}
/* 001b7510 000000c4 */ static loadfilesizeatom() {}
/* 001b75d8 00000060 */ FILE_loadsize() {}
/* 001b7638 0000005c */ FILE_loadsizez() {}
/* 001b7698 00000098 */ static loadfileatatom() {}
/* 001b7730 0000004c */ FILE_loadat() {}
/* 001b7780 00000048 */ FILE_loadatz() {}
/* 001b77c8 00000094 */ static scatterloadatom() {}
/* 001b7860 00000068 */ FILE_scatterload() {}
/* 001b78c8 00000068 */ FILE_scatterloadz() {}
/* 001b7930 00000160 */ static loadbigheaderatom() {}
/* 001b7a90 00000040 */ FILE_loadbigheader() {}
/* 001b7ad0 0000003c */ FILE_loadbigheaderz() {}
/* 001b7b10 000000e8 */ FILE_save() {}
/* 001b7bf8 000000fc */ FILE_savez() {}

// *****************************************************************************
// FILE -- src/file/cmn/locatbig.cpp
// *****************************************************************************

/* 001b7cf8 00000070 */ static BIG_stricmp() {}
/* 001b7d68 00000080 */ BIG_typeofheader() {}
/* 001b7de8 00000090 */ BIG_sizeofheader() {}
/* 001b7e78 0000012c */ static BIG_debuginfo() {}
/* 001b7fa8 00000028 */ BIG_version() {}
/* 001b7fd0 00000574 */ BIG_locateentryz() {}
/* 001b8548 0000001c */ BIG_locateentry() {}
/* 001b8568 00000030 */ BIG_locateoffset() {}
/* 001b8598 00000030 */ BIG_locateoffsetz() {}
/* 001b85c8 00000048 */ BIG_pointer() {}
/* 001b8610 00000048 */ BIG_pointerz() {}
/* 001b8658 00000030 */ BIG_locate() {}
/* 001b8688 00000030 */ BIG_locatez() {}
/* 001b86b8 00000090 */ BIG_locatemany() {}
/* 001b8748 0000008c */ BIG_count() {}
/* 001b87d8 00000054 */ BIG_name() {}
/* 001b8830 00000054 */ BIG_namez() {}

// *****************************************************************************
// FILE -- src/file/ps2/filedev.cpp
// *****************************************************************************

/* 001b8888 00000078 */ FILEDEV_init() {}
/* 001b8900 00000170 */ FILEDEV_resolvename() {}
/* 001b8a70 000000ec */ FILEDEV_open() {}
/* 001b8b60 00000058 */ FILEDEV_close() {}
/* 001b8bb8 0000005c */ FILEDEV_readreq() {}
/* 001b8c18 000000c8 */ static FILEDEV_readexec() {}
/* 001b8ce0 0000006c */ FILEDEV_write() {}
/* 001b8d50 00000050 */ FILEDEV_getsize() {}
/* 001b8da0 0000004c */ FILEDEV_getstartsector() {}

// *****************************************************************************
// FILE -- src/file/ps2/cddrv.cpp
// *****************************************************************************

/* 001b8df0 00000024 */ static CB_DelayTh() {}
/* 001b8e18 00000068 */ static Sample_DelayThread() {}
/* 001b8e80 000000e0 */ readsectorB() {}
/* 001b8f60 0000005c */ static CDVD_hashname() {}
/* 001b8fc0 00000210 */ loaddirinfo() {}
/* 001b91d0 00000040 */ static fi_cmp() {}
/* 001b9210 0000012c */ static ComputeHashNames() {}
/* 001b9340 00000024 */ static read_end_func() {}
/* 001b9368 000000dc */ StartReadWithRetry() {}
/* 001b9448 00000194 */ static CdReadCompleteThread() {}
/* 001b95e0 00000288 */ CDVD_resettask() {}
/* 001b9868 0000003c */ CDVD_watchdogtimeout() {}
/* 001b98a8 00000470 */ CDVD_init() {}
/* 001b9d18 00000058 */ CDVD_shutdown() {}
/* 001b9d70 00000140 */ CDVD_open() {}
/* 001b9eb0 00000290 */ CDVD_read() {}
/* 001ba140 0000008c */ CDVD_getattrfromhandle() {}
/* 001ba1d0 00000050 */ CDVD_getinfo() {}

// *****************************************************************************
// FILE -- src/math/cmn/mat4math.cpp
// *****************************************************************************

/* 001ba220 000004a0 */ MATRIX4_mult() {}
/* 001ba6c0 000001d0 */ MATRIX4_vect3mult() {}
/* 001ba890 00000164 */ MATRIX4_vect3mult3() {}
/* 001ba9f8 00000204 */ MATRIX4_vect4mult() {}
/* 001bac00 0000018c */ MATRIX4_vect4mult3() {}

// *****************************************************************************
// FILE -- src/math/cmn/rmv3math.cpp
// *****************************************************************************

/* 001bad90 0000004c */ v3crossprod() {}
/* 001bade0 00000034 */ v3unitcrossprod() {}
/* 001bae18 00000060 */ v3unit() {}
/* 001bae78 00000030 */ v3dotprod() {}
/* 001baea8 00000050 */ v3dot() {}
/* 001baef8 00000048 */ v3directed() {}

// *****************************************************************************
// FILE -- src/math/cmn/rmv3arit.cpp
// *****************************************************************************

/* 001baf40 00000050 */ v3add() {}
/* 001baf90 00000050 */ v3sub() {}
/* 001bafe0 00000044 */ v3scale() {}
/* 001bb028 00000050 */ v3distance() {}

// *****************************************************************************
// FILE -- src/math/cmn/rmv3flot.cpp
// *****************************************************************************

/* 001bb078 00000064 */ v3angle() {}
/* 001bb0e0 0000002c */ v3length() {}

// *****************************************************************************
// FILE -- src/system/cmn/exit.cpp
// *****************************************************************************

/* 001bb110 00000054 */ REAL_restore() {}
/* 001bb168 00000020 */ REAL_exit() {}
/* 001bb188 00000070 */ REAL_addexit() {}
/* 001bb1f8 00000050 */ REAL_removeexit() {}

// *****************************************************************************
// FILE -- src/system/cmn/cpudetect.cpp
// *****************************************************************************

/* 001bb248 00000034 */ CPU_detect() {}
/* 001bb280 0000000c */ CPU_getinfo() {}

// *****************************************************************************
// FILE -- src/system/ps2/inittmr.cpp
// *****************************************************************************

/* 001bb290 00000274 */ TIMER_init() {}
/* 001bb508 00000050 */ TIMER_restore() {}
/* 001bb558 000000dc */ static tmrint() {}

// *****************************************************************************
// FILE -- src/system/cmn/timer.cpp
// *****************************************************************************

/* 001bb638 00000010 */ TIMER_gettick() {}
/* 001bb648 00000040 */ TIMER_elapsedticks() {}
/* 001bb688 0000001c */ TIMER_reset() {}
/* 001bb6a8 00000038 */ TIMER_settimeout() {}
/* 001bb6e0 0000003c */ TIMER_waittimeout() {}
/* 001bb720 00000030 */ TIMER_testtimeout() {}
/* 001bb750 00000054 */ TIMER_waitticks() {}
/* 001bb7a8 0000000c */ TIMER_getfrequency() {}

// *****************************************************************************
// FILE -- src/system/ps2/threads.cpp
// *****************************************************************************

/* 001bb7b8 0000004c */ THREAD_init() {}
/* 001bb808 00000030 */ THREAD_restore() {}
/* 001bb838 0000002c */ THREAD_create() {}
/* 001bb868 000000d4 */ THREAD_createparam() {}
/* 001bb940 000000c8 */ THREAD_destroy() {}
/* 001bba08 0000001c */ THREAD_remove() {}
/* 001bba28 00000008 */ THREAD_release() {}
/* 001bba30 0000001c */ static alarmhandler() {}
/* 001bba50 000000e4 */ THREAD_yieldticks() {}
/* 001bbb38 000000a8 */ THREAD_yield() {}
/* 001bbbe0 00000054 */ THREAD_iscurrent() {}
/* 001bbc38 0000006c */ THREAD_getpriority() {}
/* 001bbca8 00000078 */ THREAD_setpriority() {}
/* 001bbd20 00000060 */ THREAD_testexit() {}
/* 001bbd80 00000098 */ THREAD_waitexit() {}
/* 001bbe18 0000001c */ THREAD_exit() {}

// *****************************************************************************
// FILE -- src/system/cmn/mutex2.cpp
// *****************************************************************************

/* 001bbe38 00000044 */ MUTEX_create() {}
/* 001bbe80 0000001c */ MUTEX_destroy() {}
/* 001bbea0 00000060 */ MUTEX_lock() {}
/* 001bbf00 0000006c */ MUTEX_unlock() {}

// *****************************************************************************
// FILE -- src/system/cmn/systask.cpp
// *****************************************************************************

/* 001bbf70 00000028 */ SYNCTASK_init() {}
/* 001bbf98 000000cc */ SYNCTASK_add() {}
/* 001bc068 00000060 */ SYNCTASK_del() {}
/* 001bc0c8 000000e8 */ SYNCTASK_run() {}
/* 001bc1b0 00000064 */ abortablewait() {}

// *****************************************************************************
// FILE -- src/system/cmn/printstr.cpp
// *****************************************************************************

/* 001bc218 00000030 */ static PRINT_movedevice() {}
/* 001bc248 000000b8 */ static PRINT_movechannel() {}
/* 001bc300 00000100 */ PRINT_init() {}
/* 001bc400 00000018 */ PRINT_restore() {}
/* 001bc418 000000b8 */ PRINT_vstring() {}
/* 001bc4d0 00000044 */ PRINT_string() {}
/* 001bc518 00000050 */ PRINT_string() {}
/* 001bc568 0000006c */ PRINT_setdevicestate() {}
/* 001bc5d8 00000050 */ PRINT_getdevicestate() {}
/* 001bc628 0000006c */ PRINT_setdevicename() {}
/* 001bc698 0000004c */ PRINT_setchannelstate() {}
/* 001bc6e8 0000004c */ PRINT_getchannelstate() {}
/* 001bc738 00000054 */ PRINT_setchannelname() {}
/* 001bc790 0000007c */ PRINT_todevice() {}

// *****************************************************************************
// FILE -- src/system/cmn/abortmsg.cpp
// *****************************************************************************

/* 001bc810 00000078 */ REAL_abortmessage() {}
/* 001bc888 000000a0 */ SYSTEM_abortmessage() {}

// *****************************************************************************
// FILE -- src/system/cmn/memclear.cpp
// *****************************************************************************

/* 001bc928 00000020 */ MEM_clear() {}

// *****************************************************************************
// FILE -- src/system/cmn/validadr.cpp
// *****************************************************************************

/* 001bc948 00000070 */ MEMSYS_validaddress() {}

// *****************************************************************************
// FILE -- src/system/cmn/crc.cpp
// *****************************************************************************

/* 001bc9b8 00000054 */ MEM_crc16() {}

// *****************************************************************************
// FILE -- src/system/ps2/printps2.cpp
// *****************************************************************************

/* 001bca10 0000001c */ static PRINT_debuggerfunc() {}

// *****************************************************************************
// FILE -- src/system/cmn/memfill.cpp
// *****************************************************************************

/* 001bca30 0000012c */ MEM_fill() {}

// *****************************************************************************
// FILE -- src/system/ps2/memcopy.s
// *****************************************************************************

/* 001bcb5c 00000118 */ MEM_copy() {}

// *****************************************************************************
// FILE -- src/system/ps2/videomode.cpp
// *****************************************************************************

/* 001bcc78 00000060 */ TIMER_setvideomode() {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/s_floor.c
// *****************************************************************************

/* 001bccd8 000001d8 */ double floor(/* a1 5 */ double x) {
	/* s0 16 */ __int32_t i0;
	/* s2 18 */ __int32_t i1;
	/* s1 17 */ __int32_t j0;
	/* s3 19 */ __uint32_t i;
	/* v1 3 */ __uint32_t j;
	/* v0 2 */ ieee_double_shape_type ew_u;
	/* s4 20 */ ieee_double_shape_type iw_u;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/sf_atan.c
// *****************************************************************************

/* 001bceb0 000002a8 */ float atanf(/* f12 50 */ float x) {
	/* f0 38 */ float w;
	/* f0 38 */ float s1;
	/* f1 39 */ float s2;
	/* f10 48 */ float z;
	/* s0 16 */ __int32_t ix;
	/* s1 17 */ __int32_t hx;
	/* a0 4 */ __int32_t id;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/sf_ceil.c
// *****************************************************************************

/* 001bd158 000000d4 */ float ceilf(/* f12 50 */ float x) {
	/* a0 4 */ __int32_t i0;
	/* a1 5 */ __int32_t j0;
	/* a2 6 */ __uint32_t i;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/sf_cos.c
// *****************************************************************************

/* 001bd230 000000e8 */ float cosf(/* f12 50 */ float x) {
	/* 0x0(sp) */ float y[2];
	/* a0 4 */ __int32_t ix;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/sf_fabs.c
// *****************************************************************************

/* 001bd318 0000001c */ float fabsf(/* v1 3 */ float x) {
	/* v1 3 */ __uint32_t ix;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/sf_floor.c
// *****************************************************************************

/* 001bd338 000000e4 */ float floorf(/* f12 50 */ float x) {
	/* a0 4 */ __int32_t i0;
	/* a1 5 */ __int32_t j0;
	/* a2 6 */ __uint32_t i;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/sf_sin.c
// *****************************************************************************

/* 001bd420 000000f0 */ float sinf(/* f12 50 */ float x) {
	/* 0x0(sp) */ float y[2];
	/* a0 4 */ __int32_t ix;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/sf_tan.c
// *****************************************************************************

/* 001bd510 00000088 */ float tanf(/* f12 50 */ float x) {
	/* 0x0(sp) */ float y[2];
	/* a0 4 */ __int32_t ix;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/w_fmod.c
// *****************************************************************************

/* 001bd598 0000011c */ double fmod(/* s1 17 */ double x, /* s0 16 */ double y) {
	/* s2 18 */ double z;
	/* 0x0(sp) */ exception exc;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/wf_acos.c
// *****************************************************************************

/* 001bd6b8 000000fc */ float acosf(/* f20 58 */ float x) {
	/* f21 59 */ float z;
	/* 0x0(sp) */ exception exc;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/wf_asin.c
// *****************************************************************************

/* 001bd7b8 000000fc */ float asinf(/* f20 58 */ float x) {
	/* f21 59 */ float z;
	/* 0x0(sp) */ exception exc;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/wf_atan2.c
// *****************************************************************************

/* 001bd8b8 00000128 */ float atan2f(/* f20 58 */ float y, /* f21 59 */ float x) {
	/* f22 60 */ float z;
	/* 0x0(sp) */ exception exc;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/wf_exp.c
// *****************************************************************************

/* 001bd9e0 00000174 */ float expf(/* f20 58 */ float x) {
	/* f21 59 */ float z;
	/* 0x0(sp) */ exception exc;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/wf_fmod.c
// *****************************************************************************

/* 001bdb58 0000013c */ float fmodf(/* f21 59 */ float x, /* f20 58 */ float y) {
	/* f22 60 */ float z;
	/* 0x0(sp) */ exception exc;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/wf_log.c
// *****************************************************************************

/* 001bdc98 00000148 */ float logf(/* f20 58 */ float x) {
	/* f21 59 */ float z;
	/* 0x0(sp) */ exception exc;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/wf_pow.c
// *****************************************************************************

/* 001bdde0 00000508 */ float powf(/* f23 61 */ float x, /* f20 58 */ float y) {
	/* f21 59 */ float z;
	/* 0x0(sp) */ exception exc;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/e_fmod.c
// *****************************************************************************

/* 001be2e8 000003f0 */ double __ieee754_fmod(/* a0 4 */ double x, /* a1 5 */ double y) {
	/* a2 6 */ __int32_t n;
	/* a3 7 */ __int32_t hx;
	/* t1 9 */ __int32_t hy;
	/* v1 3 */ __int32_t hz;
	/* a1 5 */ __int32_t ix;
	/* a0 4 */ __int32_t iy;
	/* t3 11 */ __int32_t sx;
	/* v0 2 */ __int32_t i;
	/* t0 8 */ __uint32_t lx;
	/* t2 10 */ __uint32_t ly;
	/* a1 5 */ __uint32_t lz;
	/* v0 2 */ ieee_double_shape_type ew_u;
	/* v0 2 */ ieee_double_shape_type ew_u;
	/* t6 14 */ ieee_double_shape_type iw_u;
	/* t7 15 */ ieee_double_shape_type iw_u;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/ef_acos.c
// *****************************************************************************

/* 001be6d8 00000430 */ float __ieee754_acosf(/* f11 49 */ float x) {
	/* f21 59 */ float z;
	/* f20 58 */ float p;
	/* f22 60 */ float q;
	/* f12 50 */ float r;
	/* f0 38 */ float w;
	/* f10 48 */ float s;
	/* f2 40 */ float c;
	/* v1 3 */ float df;
	/* a0 4 */ __int32_t hx;
	/* v1 3 */ __int32_t ix;
	/* v1 3 */ __int32_t idf;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/ef_asin.c
// *****************************************************************************

/* 001beb08 0000039c */ float __ieee754_asinf(/* f12 50 */ float x) {
	/* f20 58 */ float t;
	/* f8 46 */ float w;
	/* f21 59 */ float p;
	/* f22 60 */ float q;
	/* f0 38 */ float c;
	/* f5 43 */ float r;
	/* f7 45 */ float s;
	/* s1 17 */ __int32_t hx;
	/* s0 16 */ __int32_t ix;
	/* v1 3 */ __int32_t iw;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/ef_atan2.c
// *****************************************************************************

/* 001beea8 000002e8 */ float __ieee754_atan2f(/* f0 38 */ float y, /* t0 8 */ float x) {
	/* v1 3 */ float z;
	/* v0 2 */ __int32_t k;
	/* s0 16 */ __int32_t m;
	/* a3 7 */ __int32_t hx;
	/* a2 6 */ __int32_t hy;
	/* a1 5 */ __int32_t ix;
	/* a0 4 */ __int32_t iy;
	/* v1 3 */ __uint32_t zh;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/ef_exp.c
// *****************************************************************************

/* 001bf190 000002b4 */ float __ieee754_expf(/* f12 50 */ float x) {
	/* v1 3 */ float y;
	/* f7 45 */ float hi;
	/* f6 44 */ float lo;
	/* f3 41 */ float c;
	/* f5 43 */ float t;
	/* a2 6 */ __int32_t k;
	/* a3 7 */ __int32_t xsb;
	/* a0 4 */ __uint32_t hx;
	/* v1 3 */ __uint32_t hy;
	/* v0 2 */ __uint32_t hy;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/ef_fmod.c
// *****************************************************************************

/* 001bf448 00000250 */ float __ieee754_fmodf(/* a0 4 */ float x, /* a3 7 */ float y) {
	/* a0 4 */ __int32_t n;
	/* a1 5 */ __int32_t hx;
	/* a2 6 */ __int32_t hy;
	/* v1 3 */ __int32_t hz;
	/* t0 8 */ __int32_t ix;
	/* a3 7 */ __int32_t iy;
	/* t1 9 */ __int32_t sx;
	/* v0 2 */ __int32_t i;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/ef_log.c
// *****************************************************************************

/* 001bf698 00000324 */ float __ieee754_logf(/* a0 4 */ float x) {
	/* f4 42 */ float hfsq;
	/* f12 50 */ float f;
	/* f10 48 */ float s;
	/* f3 41 */ float z;
	/* f2 40 */ float R;
	/* f0 38 */ float w;
	/* f0 38 */ float t1;
	/* f3 41 */ float t2;
	/* f9 47 */ float dk;
	/* t0 8 */ __int32_t k;
	/* a3 7 */ __int32_t ix;
	/* v1 3 */ __int32_t i;
	/* v0 2 */ __int32_t j;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/ef_pow.c
// *****************************************************************************

/* 001bf9c0 0000088c */ float __ieee754_powf(/* f20 58 */ float x, /* f21 59 */ float y) {
	/* f13 51 */ float z;
	/* f12 50 */ float ax;
	/* f2 40 */ float z_h;
	/* f1 39 */ float z_l;
	/* f7 45 */ float p_h;
	/* f6 44 */ float p_l;
	/* v1 3 */ float y1;
	/* a0 4 */ float t1;
	/* f1 39 */ float t2;
	/* f0 38 */ float r;
	/* f20 58 */ float s;
	/* f12 50 */ float t;
	/* f15 53 */ float u;
	/* f14 52 */ float v;
	/* f9 47 */ float w;
	/* v0 2 */ __int32_t i;
	/* a1 5 */ __int32_t j;
	/* t0 8 */ __int32_t k;
	/* s4 20 */ __int32_t yisint;
	/* t1 9 */ __int32_t n;
	/* s3 19 */ __int32_t hx;
	/* s2 18 */ __int32_t hy;
	/* s0 16 */ __int32_t ix;
	/* s1 17 */ __int32_t iy;
	/* v1 3 */ __int32_t is;
	/* f11 49 */ float s2;
	/* f5 43 */ float s_h;
	/* f1 39 */ float s_l;
	/* v1 3 */ float t_h;
	/* f2 40 */ float t_l;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/ef_rem_pio2.c
// *****************************************************************************

/* 001c0250 000003e0 */ __int32_t __ieee754_rem_pio2f(/* f12 50 */ float x, /* s1 17 */ float *y) {
	/* f12 50 */ float z;
	/* f3 41 */ float w;
	/* f5 43 */ float t;
	/* f4 42 */ float r;
	/* f6 44 */ float fn;
	/* 0x0(sp) */ float tx[3];
	/* v1 3 */ __int32_t i;
	/* a0 4 */ __int32_t j;
	/* a1 5 */ __int32_t n;
	/* s0 16 */ __int32_t ix;
	/* s2 18 */ __int32_t hx;
	/* a2 6 */ int e0;
	/* a3 7 */ int nx;
	/* v0 2 */ __uint32_t high;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/ef_sqrt.c
// *****************************************************************************

/* 001c0630 00000138 */ float __ieee754_sqrtf(/* f12 50 */ float x) {
	/* v0 2 */ float z;
	/* a2 6 */ __uint32_t r;
	/* a1 5 */ __int32_t ix;
	/* a3 7 */ __int32_t s;
	/* a0 4 */ __int32_t q;
	/* a2 6 */ __int32_t m;
	/* v1 3 */ __int32_t t;
	/* a0 4 */ __int32_t i;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/kf_cos.c
// *****************************************************************************

/* 001c0768 00000158 */ float __kernel_cosf(/* f12 50 */ float x, /* f13 51 */ float y) {
	/* f1 39 */ float a;
	/* f0 38 */ float hz;
	/* f6 44 */ float z;
	/* f1 39 */ float r;
	/* v0 2 */ float qx;
	/* a0 4 */ __int32_t ix;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/kf_rem_pio2.c
// *****************************************************************************

/* 001c08c0 0000094c */ int __kernel_rem_pio2f(/* 0x140(sp) */ float *x, /* s7 23 */ float *y, /* a2 6 */ int e0, /* a3 7 */ int nx, /* 0x144(sp) */ int prec, /* 0x148(sp) */ __int32_t *ipio2) {
	/* s0 16 */ __int32_t jz;
	/* s1 17 */ __int32_t jx;
	/* s6 22 */ __int32_t jv;
	/* s4 20 */ __int32_t jp;
	/* a3 7 */ __int32_t carry;
	/* s3 19 */ __int32_t n;
	/* 0x0(sp) */ __int32_t iq[20];
	/* a2 6 */ __int32_t i;
	/* a1 5 */ __int32_t j;
	/* t0 8 */ __int32_t k;
	/* a3 7 */ __int32_t m;
	/* s2 18 */ __int32_t q0;
	/* 0x14c(sp) */ __int32_t ih;
	/* f20 58 */ float z;
	/* f2 40 */ float fw;
	/* 0x50(sp) */ float f[20];
	/* 0xa0(sp) */ float fq[20];
	/* 0xf0(sp) */ float q[20];
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/kf_sin.c
// *****************************************************************************

/* 001c1210 00000104 */ float __kernel_sinf(/* f12 50 */ float x, /* f13 51 */ float y, /* a1 5 */ int iy) {
	/* f5 43 */ float z;
	/* f1 39 */ float r;
	/* f6 44 */ float v;
	/* a0 4 */ __int32_t ix;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/kf_tan.c
// *****************************************************************************

/* 001c1318 00000294 */ float __kernel_tanf(/* f12 50 */ float x, /* f13 51 */ float y, /* a0 4 */ int iy) {
	/* f14 52 */ float z;
	/* f3 41 */ float r;
	/* f4 42 */ float v;
	/* f11 49 */ float w;
	/* f15 53 */ float s;
	/* a1 5 */ __int32_t ix;
	/* a2 6 */ __int32_t hx;
	/* a0 4 */ float a;
	/* v1 3 */ float t;
	/* v1 3 */ __int32_t i;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/s_isnan.c
// *****************************************************************************

/* 001c15b0 00000038 */ int isnan(/* a0 4 */ double x) {
	/* a0 4 */ __int32_t hx;
	/* a0 4 */ ieee_double_shape_type ew_u;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/common/s_matherr.c
// *****************************************************************************

/* 001c15e8 00000024 */ int matherr(/* a0 4 */ exception *x) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/common/s_rint.c
// *****************************************************************************

/* 001c1610 000001fc */ double rint(/* a1 5 */ double x) {
	/* a2 6 */ __int32_t i0;
	/* t0 8 */ __int32_t j0;
	/* s3 19 */ __int32_t sx;
	/* a0 4 */ __uint32_t i;
	/* a3 7 */ __uint32_t i1;
	/* a0 4 */ double t;
	/* v0 2 */ ieee_double_shape_type ew_u;
	/* v1 3 */ ieee_double_shape_type sh_u;
	/* v0 2 */ ieee_double_shape_type gh_u;
	/* a0 4 */ ieee_double_shape_type sh_u;
	/* t1 9 */ ieee_double_shape_type iw_u;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/common/sf_finite.c
// *****************************************************************************

/* 001c1810 00000024 */ int finitef(/* v1 3 */ float x) {
	/* v0 2 */ __int32_t ix;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/math/sf_isnan.c
// *****************************************************************************

/* 001c1838 00000024 */ int isnanf(/* v1 3 */ float x) {
	/* v0 2 */ __int32_t ix;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/common/sf_scalbn.c
// *****************************************************************************

/* 001c1860 00000160 */ float scalbnf(/* a1 5 */ float x, /* a0 4 */ int n) {
	/* v1 3 */ __int32_t k;
	/* a2 6 */ __int32_t ix;
}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/newlib/libm/common/sf_copysign.c
// *****************************************************************************

/* 001c19c0 00000030 */ float copysignf(/* a1 5 */ float x, /* a0 4 */ float y) {
	/* a1 5 */ __uint32_t ix;
	/* v1 3 */ __uint32_t iy;
}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c19f0 00000190 */ sceGsResetGraph() {}
/* 001c1b80 0000000c */ sceGsGetGParam() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c1b90 00000068 */ sceGsResetPath() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c1bf8 00000298 */ sceGsSetDefDBuff() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c1e90 0000005c */ sceGsSwapDBuff() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c1ef0 00000094 */ sceGsSyncV() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c1f88 00000314 */ sceGsSyncPath() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c22a0 000001e4 */ sceGsSetDefLoadImage() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c2488 0000017c */ sceGsExecLoadImage() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c2608 000000a0 */ static checkModelVersion() {}
/* 001c26a8 00000010 */ static _GetGsDxDyOffset() {}
/* 001c26b8 00000374 */ sceGsSetDefDispEnv() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c2a30 000000bc */ sceGsPutDispEnv() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c2af0 000000c8 */ sceGszbufaddr() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c2bb8 000001e4 */ sceGsSetDefDrawEnv() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c2da0 00000104 */ sceGsSetDefClear() {}

// *****************************************************************************
// FILE -- graphdev.c
// *****************************************************************************

/* 001c2ea8 000000e8 */ sceGsPutDrawEnv() {}

// *****************************************************************************
// FILE -- ../eecdvd.c
// *****************************************************************************

/* 001c2f90 00000068 */ sceCdCallback() {}
/* 001c2ff8 000000a0 */ _sceCd_cd_callback() {}
/* 001c3098 000000d0 */ _Cdvd_cbLoop() {}
/* 001c3168 000000cc */ sceCdInitEeCB() {}
/* 001c3238 000000b8 */ _sceCd_cd_read_intr() {}
/* 001c32f0 000000b8 */ static cmd_sem_init() {}
/* 001c33a8 000000a0 */ static cdvd_exit() {}
/* 001c3448 00000080 */ sceCdPOffCallback() {}
/* 001c34c8 00000054 */ static _sceCd_Poff_Intr() {}
/* 001c3520 00000078 */ static PowerOffCB() {}
/* 001c3598 00000314 */ sceCdLayerSearchFile() {}
/* 001c38b0 0000001c */ sceCdSearchFile() {}
/* 001c38d0 00000170 */ _sceCd_ncmd_prechk() {}
/* 001c3a40 00000098 */ sceCdNcmdDiskReady() {}
/* 001c3ad8 000000a0 */ sceCdSync() {}
/* 001c3b78 0000006c */ sceCdSyncS() {}
/* 001c3be8 00000170 */ _sceCd_scmd_prechk() {}
/* 001c3d58 000000b0 */ _sceCdSC() {}
/* 001c3e08 000002e0 */ sceCdInit() {}
/* 001c40e8 000001f8 */ sceCdDiskReady_old() {}
/* 001c42e0 00000204 */ sceCdDiskReady() {}

// *****************************************************************************
// FILE -- ../eecdvd.c
// *****************************************************************************

/* 001c44e8 000001e0 */ sceCdRead() {}

// *****************************************************************************
// FILE -- ../eecdvd.c
// *****************************************************************************

/* 001c46c8 00000290 */ sceCdGetToc() {}

// *****************************************************************************
// FILE -- ../eecdvd.c
// *****************************************************************************

/* 001c4958 00000098 */ sceCdGetDiskType() {}

// *****************************************************************************
// FILE -- ../eecdvd.c
// *****************************************************************************

/* 001c49f0 00000098 */ sceCdGetError() {}

// *****************************************************************************
// FILE -- ../eecdvd.c
// *****************************************************************************

/* 001c4a88 000000d0 */ sceCdMmode() {}

// *****************************************************************************
// FILE -- ../eecdvd.c
// *****************************************************************************

/* 001c4b58 000000f8 */ sceCdReadClock() {}

// *****************************************************************************
// FILE -- libscf.c
// *****************************************************************************

/* 001c4c50 00000070 */ static GetRomName() {}
/* 001c4cc0 00000040 */ static IsT10K() {}
/* 001c4d00 00000060 */ sceScfGetLanguage() {}
/* 001c4d60 0000001c */ sceScfSetT10kConfig() {}
/* 001c4d80 00000040 */ sceScfGetAspect() {}
/* 001c4dc0 0000003c */ sceScfGetSpdif() {}
/* 001c4e00 0000004c */ sceScfGetTimeZone() {}
/* 001c4e50 00000064 */ sceScfGetDateNotation() {}
/* 001c4eb8 00000068 */ sceScfGetSummerTime() {}
/* 001c4f20 00000068 */ sceScfGetTimeNotation() {}
/* 001c4f88 0000002c */ static tobcd() {}
/* 001c4fb8 00000020 */ static frombcd() {}
/* 001c4fd8 00000068 */ static convertfrombcd() {}
/* 001c5040 00000068 */ static converttobcd() {}
/* 001c50a8 000000b4 */ static adddate() {}
/* 001c5160 000000a4 */ static subdate() {}
/* 001c5208 00000030 */ static addhour() {}
/* 001c5238 00000024 */ static subhour() {}
/* 001c5260 0000008c */ static AdjustTime() {}
/* 001c52f0 00000008 */ sceScfGetGMTfromRTC() {}
/* 001c52f8 00000048 */ sceScfGetLocalTimefromRTC() {}

// *****************************************************************************
// FILE -- libdma.c
// *****************************************************************************

/* 001c5340 00000070 */ static WaitDma() {}
/* 001c53b0 00000084 */ static WatchDma() {}
/* 001c5438 00000028 */ static CheckAddress() {}
/* 001c5460 00000034 */ static memclr() {}
/* 001c5498 00000028 */ sceDmaGetChan() {}
/* 001c54c0 000000dc */ sceDmaReset() {}
/* 001c55a0 000001d8 */ sceDmaPutEnv() {}
/* 001c5778 00000038 */ sceDmaGetEnv() {}
/* 001c57b0 00000014 */ sceDmaPutStallAddr() {}
/* 001c57c8 00000068 */ sceDmaSend() {}
/* 001c5830 00000074 */ sceDmaSendN() {}
/* 001c58a8 00000074 */ sceDmaSendI() {}
/* 001c5920 00000048 */ sceDmaRecv() {}
/* 001c5968 0000007c */ sceDmaRecvN() {}
/* 001c59e8 00000080 */ sceDmaRecvI() {}
/* 001c5a68 00000038 */ sceDmaSync() {}
/* 001c5aa0 00000034 */ sceDmaWatch() {}
/* 001c5ad8 00000098 */ sceDmaPause() {}
/* 001c5b70 00000014 */ sceDmaRestart() {}

// *****************************************************************************
// FILE -- libmtap.c
// *****************************************************************************

/* 001c5b88 00000028 */ static DPRINT() {}
/* 001c5bb0 00000240 */ sceMtapInit() {}
/* 001c5df0 0000006c */ sceMtapPortOpen() {}
/* 001c5e60 0000006c */ sceMtapPortClose() {}
/* 001c5ed0 0000006c */ sceMtapGetConnection() {}
/* 001c5f40 00000070 */ sceMtapChangeThreadPriority() {}
/* 001c5fb0 00000064 */ sceMtapGetModVersion() {}

// *****************************************************************************
// FILE -- libvifpk.c
// *****************************************************************************

/* 001c6018 00000010 */ sceVif1PkInit() {}

// *****************************************************************************
// FILE -- libvifpk.c
// *****************************************************************************

/* 001c6028 00000010 */ sceVif1PkReset() {}

// *****************************************************************************
// FILE -- libvifpk.c
// *****************************************************************************

/* 001c6038 00000058 */ sceVif1PkTerminate() {}

// *****************************************************************************
// FILE -- libvifpk.c
// *****************************************************************************

/* 001c6090 00000058 */ sceVif1PkCnt() {}

// *****************************************************************************
// FILE -- libvifpk.c
// *****************************************************************************

/* 001c60e8 00000058 */ sceVif1PkEnd() {}

// *****************************************************************************
// FILE -- libvifpk.c
// *****************************************************************************

/* 001c6140 00000058 */ sceVif1PkOpenDirectCode() {}

// *****************************************************************************
// FILE -- libvifpk.c
// *****************************************************************************

/* 001c6198 0000002c */ sceVif1PkCloseDirectCode() {}

// *****************************************************************************
// FILE -- libvifpk.c
// *****************************************************************************

/* 001c61c8 00000018 */ sceVif1PkOpenGifTag() {}

// *****************************************************************************
// FILE -- libvifpk.c
// *****************************************************************************

/* 001c61e0 000000a4 */ sceVif1PkCloseGifTag() {}

// *****************************************************************************
// FILE -- libvifpk.c
// *****************************************************************************

/* 001c6288 0000007c */ sceVif1PkAlign() {}

// *****************************************************************************
// FILE -- libvifpk.c
// *****************************************************************************

/* 001c6308 00000028 */ sceVif1PkAddGsData() {}

// *****************************************************************************
// FILE -- libpad.c
// *****************************************************************************

/* 001c6330 00000114 */ static _send_to_iop() {}
/* 001c6448 00000140 */ scePadInit() {}
/* 001c6588 0000009c */ static scePadInit2() {}
/* 001c6628 00000080 */ scePadEnd() {}
/* 001c66a8 000001e4 */ scePadPortOpen() {}
/* 001c6890 000000b8 */ scePadPortClose() {}
/* 001c6948 00000060 */ static scePadGetDmaStr() {}
/* 001c69a8 00000050 */ scePadGetFrameCount() {}
/* 001c69f8 00000164 */ scePadRead() {}
/* 001c6b60 00000078 */ scePadGetState() {}
/* 001c6bd8 00000034 */ scePadStateIntToStr() {}
/* 001c6c10 00000064 */ static scePadSetReqState() {}
/* 001c6c78 00000050 */ scePadGetReqState() {}
/* 001c6cc8 00000034 */ scePadReqIntToStr() {}
/* 001c6d00 00000120 */ scePadInfoAct() {}
/* 001c6e20 0000011c */ scePadInfoComb() {}
/* 001c6f40 00000138 */ scePadInfoMode() {}
/* 001c7078 000000b4 */ scePadSetMainMode() {}
/* 001c7130 000000c0 */ scePadSetActDirect() {}
/* 001c71f0 000000d8 */ scePadSetActAlign() {}
/* 001c72c8 000000b4 */ static scePadGetButtonMask() {}
/* 001c7380 000000b0 */ static scePadSetButtonInfo() {}
/* 001c7430 0000005c */ scePadInfoPressMode() {}
/* 001c7490 00000054 */ scePadEnterPressMode() {}
/* 001c74e8 00000054 */ scePadExitPressMode() {}
/* 001c7540 000000cc */ static scePadSetVrefParam() {}
/* 001c7610 00000064 */ static scePadGetPortMax() {}
/* 001c7678 00000068 */ scePadGetSlotMax() {}
/* 001c76e0 00000064 */ static scePadGetModVersion() {}
/* 001c7748 00000068 */ scePadSetWarningLevel() {}

// *****************************************************************************
// FILE -- libvu0.c
// *****************************************************************************

/* 001c77b0 0000007c */ sceVu0ApplyMatrix() {}
/* 001c7830 00000090 */ sceVu0MulMatrix() {}
/* 001c78c0 0000006c */ sceVu0OuterProduct() {}
/* 001c7930 0000006c */ sceVu0InnerProduct() {}
/* 001c79a0 0000007c */ sceVu0Normalize() {}
/* 001c7a20 00000084 */ sceVu0TransposeMatrix() {}
/* 001c7aa8 000000ac */ sceVu0InversMatrix() {}
/* 001c7b58 00000070 */ sceVu0DivVector() {}
/* 001c7bc8 00000070 */ sceVu0DivVectorXYZ() {}
/* 001c7c38 00000088 */ sceVu0InterVector() {}
/* 001c7cc0 00000064 */ sceVu0AddVector() {}
/* 001c7d28 00000064 */ sceVu0SubVector() {}
/* 001c7d90 00000064 */ sceVu0MulVector() {}
/* 001c7df8 00000068 */ sceVu0ScaleVector() {}
/* 001c7e60 0000007c */ sceVu0TransMatrix() {}
/* 001c7ee0 0000004c */ sceVu0CopyVector() {}
/* 001c7f30 00000064 */ sceVu0CopyMatrix() {}
/* 001c7f98 00000050 */ sceVu0FTOI4Vector() {}
/* 001c7fe8 00000050 */ sceVu0FTOI0Vector() {}
/* 001c8038 00000050 */ sceVu0ITOF4Vector() {}
/* 001c8088 00000050 */ sceVu0ITOF0Vector() {}
/* 001c80d8 00000058 */ sceVu0UnitMatrix() {}
/* 001c8130 00000074 */ static _sceVu0ecossin() {}
/* 001c81a8 000000f0 */ sceVu0RotMatrixZ() {}
/* 001c8298 000000f4 */ sceVu0RotMatrixX() {}
/* 001c8390 000000f4 */ sceVu0RotMatrixY() {}
/* 001c8488 00000050 */ sceVu0RotMatrix() {}
/* 001c84d8 00000084 */ sceVu0ClampVector() {}
/* 001c8560 000000b0 */ sceVu0CameraMatrix() {}
/* 001c8610 000000c0 */ sceVu0NormalLightMatrix() {}
/* 001c86d0 00000068 */ sceVu0LightColorMatrix() {}
/* 001c8738 00000104 */ sceVu0ViewScreenMatrix() {}
/* 001c8840 00000190 */ sceVu0DropShadowMatrix() {}
/* 001c89d0 000000c4 */ sceVu0RotTransPersN() {}
/* 001c8a98 000000a4 */ sceVu0RotTransPers() {}
/* 001c8b40 0000001c */ sceVu0CopyVectorXYZ() {}
/* 001c8b60 0000008c */ sceVu0InterVectorXYZ() {}
/* 001c8bf0 00000068 */ sceVu0ScaleVectorXYZ() {}
/* 001c8c58 0000007c */ sceVu0ClipScreen() {}
/* 001c8cd8 000000ac */ sceVu0ClipScreen3() {}
/* 001c8d88 000000ec */ sceVu0ClipAll() {}
/* 001c8e78 00000080 */ sceVpu0Reset() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoAptFunctionInterface.cpp
// *****************************************************************************

/* 001c8f20 0000014c */ EASO::EASO_InitAptInterface() {}
/* 001c9070 00000028 */ EASO::EASO_IsEasoAnimationFile() {}
/* 001c9098 000000c4 */ EASO::EASO_LoadVariables() {}
/* 001c9160 000000c4 */ EASO::EASO_Extern() {}
/* 001c9228 00000138 */ EASO::EASO_Function() {}
/* 001c9360 00000028 */ EASO::EASO_IsEasoAptInterfaceFunction() {}
/* 001c9388 000000c0 */ EASO::EASO_PostCBEvent() {}
/* 001c9448 000000c0 */ EASO::EASO_PostEvent() {}
/* 001c9508 000000a0 */ EASO::EASO_InitLobbyRef() {}
/* 001c95a8 00000024 */ EASO::EASO_GameOverCallback() {}
/* 001c95d0 00000028 */ EASO::EASO_DnasElfLoadComplete() {}
/* 001c95f8 00000028 */ EASO::EASO_AuthDataLoadComplete() {}
/* 001c9620 00000028 */ EASO::EASO_EncodedDataLoadComplete() {}
/* 001c9648 0000001c */ EASO::EASO_Update() {}
/* 001c9668 00000024 */ EASO::EASO_GetFileData() {}
/* 001c9690 00000034 */ EASO::EASO_GetNewsFileValue() {}
/* 001c96c8 00000090 */ EASO::EASO_ResetOnlineState() {}
/* 001c9758 00000020 */ EASO::EASO_GetLobbyApiRefT() {}
/* 001c9778 00000008 */ EASO::EASO_IsXGameInvitePending() {}
/* 001c9780 00000010 */ EASO::EASO_GetXGameInvite() {}
/* 001c9790 00000010 */ EASO::EASO_GetHomeUser() {}
/* 001c97a0 00000010 */ EASO::EASO_GetAwayUser() {}
/* 001c97b0 0000000c */ EASO::EASO_GetHomeScore() {}
/* 001c97c0 0000000c */ EASO::EASO_GetAwayScore() {}
/* 001c97d0 0000000c */ EASO::EASO_GetTID() {}
/* 001c97e0 0000000c */ EASO::EASO_GetTMID() {}
/* 001c97f0 000001b0 */ EASO::EASO_LoadNetworkModules() {}
/* 001c99a0 00000458 */ EASO::GlobalPrivateMessageHandler() {}
/* 001c9df8 00000050 */ EASO::RegGlobalPrivateMsgHandler() {}
/* 001c9e48 00000048 */ EASO::ClrGlobalPrivateMsgHandler() {}
/* 001c9e90 00000008 */ EASO::GlobalCBTEventHandler() {}
/* 001c9e98 00000050 */ EASO::RegisterGlobalUserOnChangeEvent() {}
/* 001c9ee8 00000048 */ EASO::ClrGlobalUserOnChangeEvent() {}
/* 001c9f30 000001c4 */ EASO::GlobalEventHandler() {}
/* 001ca0f8 00000050 */ EASO::RegisterGlobalcallback() {}
/* 001ca148 00000048 */ EASO::ClearGlobalcallback() {}
/* 001ca190 00000060 */ EASO::GlobalCleanup() {}
/* 001ca1f0 00000020 */ EASO::GetVoipStatus() {}
/* 001ca210 00000020 */ EASO::VoipMangleUpdate() {}
/* 001ca230 00000088 */ EASO::Split() {}
/* 001ca2b8 0000000c */ EASO::ResetMCCount() {}
/* 001ca2c8 00000014 */ EASO::AddMCCount() {}
/* 001ca2e0 0000000c */ EASO::GetMCCount() {}
/* 001ca2f0 000000f0 */ static EASO::_UnpackEAConnectCSV() {}
/* 001ca3e0 00000300 */ EASO::SaveEAConnectParams() {}
/* 001ca6e0 000002c4 */ EASO::FixupHost0Paths() {}
/* 001ca9a8 0000000c */ EASO::GetDASHDataPath() {}
/* 001ca9b8 0000000c */ EASO::GetDASHParams() {}
/* 001ca9c8 0000000c */ EASO::IsCDRom() {}
/* 001ca9d8 0000002c */ EASO::IsPAL() {}
/* 001caa08 00000024 */ EASO::InstallResetHandler() {}
/* 001caa30 0000001c */ EASO::RemoveResetHandler() {}
/* 001caa50 00000034 */ EASO::GetBasePath() {}
/* 001caa88 00000068 */ EASO::GetDASHLibPath() {}
/* 001caaf0 0000001c */ EASO::GetDirtyIRXPath() {}
/* 001cab10 0000000c */ EASO::GetDASHElfPath() {}
/* 001cab20 00000058 */ EASO::GetNetGuiPath() {}
/* 001cab78 0000000c */ EASO::GetDirtyIRXName() {}
/* 001cab88 0000000c */ EASO::GetIOPImageName() {}
/* 001cab98 00000058 */ EASO::GetEncodedDNASFilePath() {}
/* 001cabf0 0000000c */ EASO::GetEncodedDNASKey() {}
/* 001cac00 0000000c */ EASO::GetGameName() {}
/* 001cac10 0000000c */ EASO::GetGameSLUS() {}
/* 001cac20 0000000c */ EASO::GetGameELFPath() {}
/* 001cac30 0000000c */ EASO::GetGameELFParams() {}
/* 001cac40 0000000c */ EASO::GetServerHost() {}
/* 001cac50 0000000c */ EASO::GetServerPort() {}
/* 001cac60 0000000c */ EASO::GetLanguage() {}
/* 001cac70 0000000c */ EASO::GetDnasRegionNum() {}
/* 001cac80 0000000c */ EASO::GetDNASPassPhrase() {}
/* 001cac90 0000000c */ EASO::GetGameId() {}
/* 001caca0 0000000c */ EASO::GetGameSKU() {}
/* 001cacb0 00000028 */ EASO::GetNoModem() {}
/* 001cacd8 00000024 */ EASO::IsEnableDebug() {}
/* 001cad00 0000000c */ EASO::GetPreLoadedBundleFile() {}
/* 001cad10 0000000c */ EASO::SetPreLoadedBundleFile() {}
/* 001cad20 0000000c */ EASO::GetPreLoadedLocFile() {}
/* 001cad30 0000000c */ EASO::SetPreLoadedLocFile() {}
/* 001cad40 0000000c */ EASO::GetPreLoadedModuleFile() {}
/* 001cad50 0000000c */ EASO::SetPreLoadedModuleFile() {}
/* 001cad60 0000011c */ EASO::EASO_LoadModule() {}
/* 001cae80 00000140 */ static __static_initialization_and_destruction_0() {}
/* 001cafc0 00000020 */ _GLOBAL_$I$C__Work_dash_cfe_packages_easoApt_DASH_2006_source_easoAptFunctionInterface.cpp5Iaaaa() {}
/* 001cafe0 00000020 */ _GLOBAL_$D$C__Work_dash_cfe_packages_easoApt_DASH_2006_source_easoAptFunctionInterface.cpp91aaaa() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000054 */ EASO::EasoHub::Instance() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}
/* ffffffff 00000024 */ EASO::EasoHub::EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000001fc */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<char *>() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::ConstDbgIterator>() {}
/* ffffffff 00000078 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> & EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=<EASO::FifaStringPolicy>() {}
/* ffffffff 000000c4 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000148 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Reserve() {}
/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000060 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator=() {}
/* ffffffff 00000154 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Erase() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}
/* ffffffff 00000114 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Swap() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/lexical_cast.h
// *****************************************************************************

/* ffffffff 000001a4 */ EASO::String::LexicalCastNamespace::LexicalCastDetail::LexicalCast_To<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >::LexicalCast_From() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/EASOString.h
// *****************************************************************************

/* ffffffff 000001e4 */ void EASO::AddPair<int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<int>() {}
/* ffffffff 000001e4 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000002a8 */ EASO::UrlEncode() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/default_policy.h
// *****************************************************************************

/* 001c8ef8 0000012c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator EASO::String::DefaultPolicy::Copy<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/netVOIP.cpp
// *****************************************************************************

/* 001cb028 00000168 */ EASO::NetVOIP::NetVOIP() {}
/* 001cb190 00000048 */ EASO::NetVOIP::~NetVOIP() {}
/* 001cb1d8 0000006c */ EASO::NetVOIP::RegisterMethod() {}
/* 001cb248 00000154 */ EASO::NetVOIP::ExecuteMethod() {}
/* 001cb3a0 00000008 */ EASO::NetVOIP::Initialize() {}
/* 001cb3a8 00000008 */ EASO::NetVOIP::Cleanup() {}
/* 001cb3b0 00000098 */ EASO::NetVOIP::Destroy() {}
/* 001cb448 00000068 */ EASO::NetVOIP::ReInitialize() {}
/* 001cb4b0 00000008 */ EASO::NetVOIP::DestroyDemangler() {}
/* 001cb4b8 0000001c */ EASO::NetVOIP::CloseXboxSession() {}
/* 001cb4d8 00000068 */ EASO::NetVOIP::HostStartVoip() {}
/* 001cb540 00000050 */ EASO::NetVOIP::ConnectToPeer() {}
/* 001cb590 000000e8 */ EASO::NetVOIP::_FindUserCallback() {}
/* 001cb678 000000d4 */ EASO::NetVOIP::CheckFireWall() {}
/* 001cb750 00000008 */ EASO::NetVOIP::ConnectDirectly() {}
/* 001cb758 000000cc */ EASO::NetVOIP::ConnectToPeerUseDemangler() {}
/* 001cb828 0000011c */ EASO::NetVOIP::ConnectToPeerCallback() {}
/* 001cb948 00000008 */ EASO::NetVOIP::ConnectToHost() {}
/* 001cb950 000000d8 */ EASO::NetVOIP::ConnectToHostUseDemangler() {}
/* 001cba28 00000068 */ EASO::NetVOIP::ConnectToHostCallback() {}
/* 001cba90 00000094 */ EASO::NetVOIP::PeerVoipGotError() {}
/* 001cbb28 000000f0 */ EASO::NetVOIP::Connect() {}
/* 001cbc18 00000088 */ EASO::NetVOIP::Disconnect() {}
/* 001cbca0 00000110 */ EASO::NetVOIP::Disconnect2() {}
/* 001cbdb0 00000098 */ EASO::NetVOIP::HostCancel() {}
/* 001cbe48 00000098 */ EASO::NetVOIP::PeerCancel() {}
/* 001cbee0 000000bc */ EASO::NetVOIP::PeerCanceledConnection() {}
/* 001cbfa0 0000013c */ EASO::NetVOIP::GetConnectState() {}
/* 001cc0e0 00000050 */ EASO::NetVOIP::DisconnectVoip() {}
/* 001cc130 000000ac */ EASO::NetVOIP::GetMyVoipStatus() {}
/* 001cc1e0 00000178 */ EASO::NetVOIP::ChangeMicState() {}
/* 001cc358 00000160 */ EASO::NetVOIP::ChangeHeadsetState() {}
/* 001cc4b8 00000124 */ EASO::NetVOIP::SetVoipOutput() {}
/* 001cc5e0 000000a0 */ EASO::NetVOIP::SetNextVoipDisconnectDialogFlag() {}
/* 001cc680 00000278 */ EASO::NetVOIP::SendSystemVoipMessage() {}
/* 001cc8f8 000002b0 */ EASO::NetVOIP::Update() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000054 */ EASO::EasoHub::Instance() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}
/* ffffffff 00000024 */ EASO::EasoHub::EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000001fc */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<char *>() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::ConstDbgIterator>() {}
/* ffffffff 00000078 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> & EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=<EASO::FifaStringPolicy>() {}
/* ffffffff 000000c4 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000148 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Reserve() {}
/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000154 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Erase() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/lexical_cast.h
// *****************************************************************************

/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 000001a4 */ EASO::String::LexicalCastNamespace::LexicalCastDetail::LexicalCast_To<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >::LexicalCast_From() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/EASOString.h
// *****************************************************************************

/* ffffffff 000001e4 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000001e4 */ void EASO::AddPair<int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<int>() {}
/* ffffffff 000001e4 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000002a8 */ EASO::UrlEncode() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/netVOIP.h
// *****************************************************************************

/* ffffffff 00000008 */ EASO::NetVOIP::GetCurrentHardwareFlags() {}
/* ffffffff 00000008 */ EASO::NetVOIP::GetOutputSetting() {}
/* ffffffff 00000008 */ EASO::NetVOIP::IsNextDisconnectSilent() {}
/* ffffffff 00000008 */ EASO::NetVOIP::SetNextDisconnectSilent() {}
/* ffffffff 00000008 */ EASO::NetVOIP::SetCurrentHardwareFlags() {}
/* ffffffff 00000008 */ EASO::NetVOIP::GetLastPollTime() {}
/* ffffffff 00000008 */ EASO::NetVOIP::SetLastPollTime() {}
/* ffffffff 00000008 */ EASO::NetVOIP::GetVoipRef() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/default_policy.h
// *****************************************************************************

/* 001cb000 0000012c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator EASO::String::DefaultPolicy::Copy<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/cfeSharedLogin.cpp
// *****************************************************************************

/* 001ccbd0 0000022c */ EASO::CFESharedLogin::CFESharedLogin() {}
/* 001cce00 00000028 */ EASO::CFESharedLogin::~CFESharedLogin() {}
/* 001cce28 0000006c */ EASO::CFESharedLogin::RegisterMethod() {}
/* 001cce98 00000154 */ EASO::CFESharedLogin::ExecuteMethod() {}
/* 001ccff0 00000074 */ EASO::CFESharedLogin::SharedLogin_CheckMC() {}
/* 001cd068 00000034 */ EASO::CFESharedLogin::SharedLogin_GetPassword() {}
/* 001cd0a0 00000034 */ EASO::CFESharedLogin::SharedLogin_GetPopupTitle() {}
/* 001cd0d8 00000090 */ EASO::CFESharedLogin::SharedLogin_GetPopupButtonCount() {}
/* 001cd168 00000054 */ EASO::CFESharedLogin::SharedLogin_GetPopupButtonLabel() {}
/* 001cd1c0 000000a8 */ EASO::CFESharedLogin::SharedLogin_GetPopupButtonResponseCode() {}
/* 001cd268 00000090 */ EASO::CFESharedLogin::SharedLogin_GetPopupDefaultButtonIndex() {}
/* 001cd2f8 00000034 */ EASO::CFESharedLogin::SharedLogin_GetUsername() {}
/* 001cd330 0000006c */ EASO::CFESharedLogin::SharedLogin_Initialize() {}
/* 001cd3a0 00000094 */ EASO::CFESharedLogin::SharedLogin_IsFinished() {}
/* 001cd438 00000094 */ EASO::CFESharedLogin::SharedLogin_IsPopupRequired() {}
/* 001cd4d0 00000094 */ EASO::CFESharedLogin::SharedLogin_IsSucceeded() {}
/* 001cd568 00000074 */ EASO::CFESharedLogin::SharedLogin_Load() {}
/* 001cd5e0 00000074 */ EASO::CFESharedLogin::SharedLogin_ResetPopupRequiredFlag() {}
/* 001cd658 0000011c */ EASO::CFESharedLogin::SharedLogin_Save() {}
/* 001cd778 00000094 */ EASO::CFESharedLogin::SharedLogin_SendUserResponse() {}
/* 001cd810 00000094 */ EASO::CFESharedLogin::SharedLogin_ShutDown() {}
/* 001cd8a8 00000074 */ EASO::CFESharedLogin::SharedLogin_Update() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000060 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator=() {}
/* 001ccba8 00000114 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Swap() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.cpp
// *****************************************************************************

/* 001cd948 0000003c */ EASO::EasoHub::SetLobbyApiRefT() {}
/* 001cd988 00000030 */ EASO::EasoHub::SetBuddyApiRefT() {}
/* 001cd9b8 00000030 */ EASO::EasoHub::SetTickerApiRefT() {}
/* 001cd9e8 00000028 */ EASO::EasoHub::SetStatbookApiRefT() {}
/* 001cda10 00000028 */ EASO::EasoHub::SetMangleApiRefT() {}
/* 001cda38 00000028 */ EASO::EasoHub::SetNetIdlePump() {}
/* 001cda60 00000120 */ EASO::EasoHub::Update() {}
/* 001cdb80 00000020 */ EASO::EasoHub::Disconnect() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000054 */ EASO::EasoHub::Instance() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}
/* 001cd920 00000024 */ EASO::EasoHub::EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/ps2/netLogin.cpp
// *****************************************************************************

/* 001cdbc8 00000414 */ EASO::NetLogin::NetLogin() {}
/* 001cdfe0 00000080 */ EASO::NetLogin::~NetLogin() {}
/* 001ce060 000000cc */ EASO::NetLogin::DestroyLoginObject() {}
/* 001ce130 00000090 */ EASO::NetLogin::Abort() {}
/* 001ce1c0 0000006c */ EASO::NetLogin::RegisterMethod() {}
/* 001ce230 00000154 */ EASO::NetLogin::ExecuteMethod() {}
/* 001ce388 000006bc */ EASO::NetLogin::SetLobbyApi() {}
/* 001cea48 0000008c */ EASO::NetLogin::AddAlertToString() {}
/* 001cead8 0000012c */ EASO::NetLogin::SetContext() {}
/* 001cec08 000001b4 */ EASO::NetLogin::LoginAPIEventCallback() {}
/* 001cedc0 00000260 */ EASO::NetLogin::Connect() {}
/* 001cf020 00000240 */ EASO::NetLogin::ConnectCallback() {}
/* 001cf260 00000114 */ EASO::NetLogin::RefreshAccountFromServer() {}
/* 001cf378 00000308 */ EASO::NetLogin::RefreshAccountFromServerCallback() {}
/* 001cf680 00000198 */ EASO::NetLogin::SendPassword() {}
/* 001cf818 000001b8 */ EASO::NetLogin::SendAccountName() {}
/* 001cf9d0 000001a4 */ EASO::NetLogin::SendParentalEmail() {}
/* 001cfb78 00000128 */ EASO::NetLogin::GetSuggestedAccountNames() {}
/* 001cfca0 00000430 */ EASO::NetLogin::CreateAccount() {}
/* 001d00d0 0000027c */ EASO::NetLogin::EditAccount() {}
/* 001d0350 000001d4 */ EASO::NetLogin::EditAccountCallback() {}
/* 001d0528 000004a8 */ EASO::NetLogin::LoginCallback() {}
/* 001d09d0 00000240 */ EASO::NetLogin::Login() {}
/* 001d0c10 000002c8 */ EASO::NetLogin::AutoLogin() {}
/* 001d0ed8 000003a0 */ EASO::NetLogin::AutoLoginCallback() {}
/* 001d1278 000000ac */ EASO::NetLogin::GetIspConnStatus() {}
/* 001d1328 000001b8 */ EASO::NetLogin::CreatePersona() {}
/* 001d14e0 0000015c */ EASO::NetLogin::GetPersonas() {}
/* 001d1640 00000120 */ EASO::NetLogin::GetSuggestedPersonas() {}
/* 001d1760 000001b8 */ EASO::NetLogin::SelectSuggestedAccountName() {}
/* 001d1918 000001b8 */ EASO::NetLogin::SelectSuggestedPersona() {}
/* 001d1ad0 000001b4 */ EASO::NetLogin::DeletePersona() {}
/* 001d1c88 000001e0 */ EASO::NetLogin::SelectPersona() {}
/* 001d1e68 0000042c */ EASO::NetLogin::SelectPersonaCallback() {}
/* 001d2298 000000f4 */ EASO::NetLogin::GetContext() {}
/* 001d2390 00000114 */ EASO::NetLogin::GoBackScreen() {}
/* 001d24a8 000000b4 */ EASO::NetLogin::GetIsNewPersona() {}
/* 001d2560 000000b4 */ EASO::NetLogin::GetIsFirstTitleLogin() {}
/* 001d2618 00000084 */ EASO::NetLogin::GetCountry() {}
/* 001d26a0 000000bc */ EASO::NetLogin::GetServerPort() {}
/* 001d2760 00000088 */ EASO::NetLogin::GetGameId() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000001fc */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<char *>() {}
/* ffffffff 00000094 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::ConstDbgIterator>() {}
/* ffffffff 00000078 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> & EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=<EASO::FifaStringPolicy>() {}
/* ffffffff 000000c4 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000148 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Reserve() {}
/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000154 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Erase() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/lexical_cast.h
// *****************************************************************************

/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 000001a4 */ EASO::String::LexicalCastNamespace::LexicalCastDetail::LexicalCast_To<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >::LexicalCast_From() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, LobbyRegContextE>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, LobbyRegContextE>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, unsigned int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, unsigned int>() {}
/* ffffffff 00000030 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *(*)>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *(*)>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/EASOString.h
// *****************************************************************************

/* ffffffff 000000e8 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000000e8 */ void EASO::AddPair<int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<int>() {}
/* ffffffff 000000e8 */ void EASO::AddPair<LobbyRegContextE>() {}
/* ffffffff 0000006c */ void EASO::AddPair<LobbyRegContextE>() {}
/* ffffffff 000000e8 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000000e8 */ void EASO::AddPair<unsigned int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<unsigned int>() {}
/* ffffffff 000000e8 */ void EASO::AddPair<char *(*)>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *(*)>() {}
/* ffffffff 000002a8 */ EASO::UrlEncode() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/ps2/netLogin.h
// *****************************************************************************

/* ffffffff 00000008 */ EASO::NetLogin::SetLobby() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/default_policy.h
// *****************************************************************************

/* 001cdba0 0000012c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator EASO::String::DefaultPolicy::Copy<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/netHttp.cpp
// *****************************************************************************

/* 001d2810 000000c0 */ EASO::NetHttp::NetHttp() {}
/* 001d28d0 00000048 */ EASO::NetHttp::~NetHttp() {}
/* 001d2918 0000006c */ EASO::NetHttp::RegisterMethod() {}
/* 001d2988 00000154 */ EASO::NetHttp::ExecuteMethod() {}
/* 001d2ae0 00000040 */ EASO::NetHttp::init() {}
/* 001d2b20 00000008 */ EASO::NetHttp::getAnimationBuffer() {}
/* 001d2b28 000000b0 */ EASO::NetHttp::reset() {}
/* 001d2bd8 00000098 */ EASO::NetHttp::ClearRosterBuffer() {}
/* 001d2c70 0000009c */ EASO::NetHttp::Cancel() {}
/* 001d2d10 000001a8 */ EASO::NetHttp::DownloadFile() {}
/* 001d2eb8 0000001c */ EASO::NetHttp::alarmhandler() {}
/* 001d2ed8 0000070c */ EASO::NetHttp::UpdateTextDownload() {}
/* 001d35e8 00000428 */ EASO::NetHttp::UpdateFileDownload() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000001fc */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<char *>() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::ConstDbgIterator>() {}
/* ffffffff 00000078 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> & EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=<EASO::FifaStringPolicy>() {}
/* ffffffff 000000c4 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000148 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Reserve() {}
/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000154 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Erase() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/lexical_cast.h
// *****************************************************************************

/* ffffffff 000001a4 */ EASO::String::LexicalCastNamespace::LexicalCastDetail::LexicalCast_To<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >::LexicalCast_From() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/EASOString.h
// *****************************************************************************

/* ffffffff 000001e4 */ void EASO::AddPair<int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<int>() {}
/* ffffffff 000001e4 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000002a8 */ EASO::UrlEncode() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/default_policy.h
// *****************************************************************************

/* 001d27e8 0000012c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator EASO::String::DefaultPolicy::Copy<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/netOffering.cpp
// *****************************************************************************

/* 001d3a38 00000064 */ static _FindShapeFile() {}
/* 001d3aa0 00000180 */ EASO::NetOffering::NetOffering() {}
/* 001d3c20 000000f4 */ EASO::NetOffering::~NetOffering() {}
/* 001d3d18 0000006c */ EASO::NetOffering::RegisterMethod() {}
/* 001d3d88 00000154 */ EASO::NetOffering::ExecuteMethod() {}
/* 001d3ee0 0000040c */ EASO::NetOffering::Initialize() {}
/* 001d42f0 000000e0 */ EASO::NetOffering::Destroy() {}
/* 001d43d0 00000d34 */ EASO::NetOffering::SetCommand() {}
/* 001d5108 00000094 */ EASO::NetOffering::GetNextChunk() {}
/* 001d51a0 00000264 */ EASO::NetOffering::GetNextChunk2() {}
/* 001d5408 00000178 */ EASO::NetOffering::GetPreviousChunk2() {}
/* 001d5580 000000cc */ EASO::NetOffering::GetPreviousChunk() {}
/* 001d5650 0000002c */ EASO::NetOffering::GetCreditType() {}
/* 001d5680 0000002c */ EASO::NetOffering::GetCountries() {}
/* 001d56b0 000004ec */ EASO::NetOffering::SetUpImage() {}
/* 001d5ba0 0000002c */ EASO::NetOffering::GetImageBuffer() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000001fc */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<char *>() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::ConstDbgIterator>() {}
/* ffffffff 00000078 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> & EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=<EASO::FifaStringPolicy>() {}
/* ffffffff 000000c4 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000148 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Reserve() {}
/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000060 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator=() {}
/* ffffffff 00000154 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Erase() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}
/* ffffffff 00000114 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Swap() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/lexical_cast.h
// *****************************************************************************

/* ffffffff 000001a4 */ EASO::String::LexicalCastNamespace::LexicalCastDetail::LexicalCast_To<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >::LexicalCast_From() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, unsigned int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, unsigned int>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/EASOString.h
// *****************************************************************************

/* ffffffff 000001e4 */ void EASO::AddPair<int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<int>() {}
/* ffffffff 000001e4 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000001e4 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000001e4 */ void EASO::AddPair<unsigned int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<unsigned int>() {}
/* ffffffff 000002a8 */ EASO::UrlEncode() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/default_policy.h
// *****************************************************************************

/* 001d3a10 0000012c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator EASO::String::DefaultPolicy::Copy<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/Delegator.cpp
// *****************************************************************************

/* 001d5bd0 00000010 */ EASO::Delegator::Delegator() {}
/* 001d5be0 00000024 */ EASO::Delegator::~Delegator() {}
/* 001d5c08 00000070 */ EASO::Delegator::RegisterObject() {}
/* 001d5c78 0000017c */ EASO::Delegator::ExecuteObjectMethod() {}
/* 001d5df8 0000015c */ EASO::Delegator::ExecuteUrl() {}
/* 001d5f58 000001a0 */ EASO::Delegator::ExecuteExternUrl() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000060 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator=() {}
/* ffffffff 00000114 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Swap() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/baseAptInterfaceClass.cpp
// *****************************************************************************

/* 001d60f8 00000018 */ EASO::BaseAptInterfaceClass::BaseAptInterfaceClass() {}
/* 001d6110 00000030 */ EASO::BaseAptInterfaceClass::~BaseAptInterfaceClass() {}
/* 001d6140 0000006c */ EASO::BaseAptInterfaceClass::ExecuteMethod() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/utils/url.cpp
// *****************************************************************************

/* 001d61b0 00000078 */ EASO::URL::URL() {}
/* 001d6228 000000b4 */ EASO::URL::~URL() {}
/* 001d62e0 00000094 */ EASO::URL::GetInt() {}
/* 001d6378 00000098 */ EASO::URL::GetLangID() {}
/* 001d6410 0000008c */ EASO::URL::GetString() {}
/* 001d64a0 00000080 */ EASO::URL::Exists() {}
/* 001d6520 000001ec */ EASO::URL::ConvertURLToPairs() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 00000060 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator=() {}
/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000114 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Swap() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/netLobby.cpp
// *****************************************************************************

/* 001d6738 00000230 */ EASO::NetLobby::NetLobby() {}
/* 001d6968 00000070 */ EASO::NetLobby::~NetLobby() {}
/* 001d69d8 0000021c */ EASO::NetLobby::Init() {}
/* 001d6bf8 00000290 */ EASO::NetLobby::SendScoreReport() {}
/* 001d6e88 0000004c */ EASO::NetLobby::ResetScoreReportValues() {}
/* 001d6ed8 00000008 */ EASO::NetLobby::ScoreReportCallback() {}
/* 001d6ee0 0000001c */ EASO::NetLobby::SetScoreReportParam() {}
/* 001d6f00 00000008 */ EASO::NetLobby::GetLobby() {}
/* 001d6f08 00000008 */ EASO::NetLobby::LobbyDebugPrint() {}
/* 001d6f10 0000006c */ EASO::NetLobby::RegisterMethod() {}
/* 001d6f80 00000154 */ EASO::NetLobby::ExecuteMethod() {}
/* 001d70d8 00000078 */ EASO::NetLobby::Update() {}
/* 001d7150 0000021c */ EASO::NetLobby::OnLobbyDisconnectEvent() {}
/* 001d7370 0000010c */ EASO::NetLobby::Disconnect() {}
/* 001d7480 00000134 */ EASO::NetLobby::GetServerUrl() {}
/* 001d75b8 000000a0 */ EASO::NetLobby::GetLastLogin() {}
/* 001d7658 00000390 */ EASO::NetLobby::LoadConfig() {}
/* 001d79e8 00000070 */ EASO::NetLobby::GetConfig() {}
/* 001d7a58 0000010c */ EASO::NetLobby::GetConfigSetting() {}
/* 001d7b68 0000011c */ EASO::NetLobby::GetOfferDate() {}
/* 001d7c88 000000c0 */ EASO::NetLobby::GetRosterVersion() {}
/* 001d7d48 00000038 */ EASO::NetLobby::Cleanup() {}
/* 001d7d80 00000094 */ EASO::NetLobby::PlayOnlineGame() {}
/* 001d7e18 00000094 */ EASO::NetLobby::ExitOnlineGame() {}
/* 001d7eb0 000000b0 */ EASO::NetLobby::SetXboxVoiceBanned() {}
/* 001d7f60 000000a8 */ EASO::NetLobby::SendKeepAlive() {}
/* 001d8008 00000084 */ EASO::NetLobby::DebugDisconnect() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000054 */ EASO::EasoHub::Instance() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}
/* ffffffff 00000024 */ EASO::EasoHub::EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 00000094 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::ConstDbgIterator>() {}
/* ffffffff 00000078 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> & EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=<EASO::FifaStringPolicy>() {}
/* ffffffff 000000c4 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000148 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Reserve() {}
/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000001fc */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<char *>() {}
/* ffffffff 00000154 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Erase() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/lexical_cast.h
// *****************************************************************************

/* ffffffff 000001a4 */ EASO::String::LexicalCastNamespace::LexicalCastDetail::LexicalCast_To<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >::LexicalCast_From() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 00000028 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, unsigned int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, unsigned int>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/EASOString.h
// *****************************************************************************

/* ffffffff 000000e8 */ void EASO::AddPair<int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<int>() {}
/* ffffffff 00000118 */ void EASO::AddPair<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >() {}
/* ffffffff 000000bc */ void EASO::AddPair<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >() {}
/* ffffffff 000000e8 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000000e8 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000000e8 */ void EASO::AddPair<unsigned int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<unsigned int>() {}
/* ffffffff 000002a8 */ EASO::UrlEncode() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/netLobby.h
// *****************************************************************************

/* ffffffff 00000008 */ EASO::NetLobby::GetNetLobby() {}
/* ffffffff 00000008 */ EASO::NetLobby::getFindUserApi() {}
/* ffffffff 00000008 */ EASO::NetLobby::isXboxVoiceBanned() {}
/* ffffffff 0000001c */ EASO::NetLobby::SetPersona() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetPersona() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetTickerServer() {}
/* ffffffff 00000020 */ EASO::NetLobby::setTickerServer() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetLKey() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetTKey() {}
/* ffffffff 0000001c */ EASO::NetLobby::setLKey() {}
/* ffffffff 0000001c */ EASO::NetLobby::setTKey() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetLastLogin() {}
/* ffffffff 00000008 */ EASO::NetLobby::SetLastLogin() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetCategoryUrl() {}
/* ffffffff 0000001c */ EASO::NetLobby::SetCategoryUrl() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetNewRoomPrefix() {}
/* ffffffff 0000001c */ EASO::NetLobby::SetNewRoomPrefix() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetMangleServer() {}
/* ffffffff 0000001c */ EASO::NetLobby::SetManglerServer() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetNewsServer() {}
/* ffffffff 0000001c */ EASO::NetLobby::SetNewsServer() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetFaqServer() {}
/* ffffffff 0000001c */ EASO::NetLobby::SetFaqServer() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetTosServer() {}
/* ffffffff 0000001c */ EASO::NetLobby::SetTosServer() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetMangleServerPort() {}
/* ffffffff 00000008 */ EASO::NetLobby::SetManglerServerPort() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetMangleTestTimeout() {}
/* ffffffff 00000008 */ EASO::NetLobby::SetMangleTestTimeout() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetMangleTimeout() {}
/* ffffffff 00000008 */ EASO::NetLobby::SetMangleTimeout() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetMangleConnTimeout() {}
/* ffffffff 00000008 */ EASO::NetLobby::SetMangleConnTimeout() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetChatMsgLimit() {}
/* ffffffff 00000008 */ EASO::NetLobby::SetChatMsgLimit() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetLobbyUpdateFreq() {}
/* ffffffff 00000008 */ EASO::NetLobby::SetLobbyUpdateFreq() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetRoomUpdateFreq() {}
/* ffffffff 00000008 */ EASO::NetLobby::SetRoomUpdateFreq() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetKeyPressTimer() {}
/* ffffffff 00000008 */ EASO::NetLobby::SetKeyPressTimer() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetXBOXDelay() {}
/* ffffffff 00000008 */ EASO::NetLobby::SetXBOXDelay() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetScoreReportName0() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetScoreReportName1() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetScoreReportScore0() {}
/* ffffffff 00000008 */ EASO::NetLobby::GetScoreReportScore1() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/default_policy.h
// *****************************************************************************

/* 001d6710 0000012c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator EASO::String::DefaultPolicy::Copy<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/ps2/netISP.cpp
// *****************************************************************************

/* 001d80b8 000002c0 */ EASO::NetISP::NetISP() {}
/* 001d8378 00000028 */ EASO::NetISP::~NetISP() {}
/* 001d83a0 0000006c */ EASO::NetISP::RegisterMethod() {}
/* 001d8410 00000154 */ EASO::NetISP::ExecuteMethod() {}
/* 001d8568 00000564 */ EASO::NetISP::Initialize() {}
/* 001d8ad0 00000098 */ EASO::NetISP::Abort() {}
/* 001d8b68 00000164 */ EASO::NetISP::Disconnect() {}
/* 001d8cd0 000000d8 */ EASO::NetISP::DestroyApi() {}
/* 001d8da8 000003dc */ EASO::NetISP::Update() {}
/* 001d9188 00000088 */ EASO::NetISP::StartDnasElfLoad() {}
/* 001d9210 000001d0 */ EASO::NetISP::LaunchNetGui() {}
/* 001d93e0 000000b8 */ EASO::NetISP::StartMemCardLoad() {}
/* 001d9498 000000f4 */ EASO::NetISP::StartNetConfigLoad() {}
/* 001d9590 00000234 */ EASO::NetISP::GetNetConfigList() {}
/* 001d97c8 000000f4 */ EASO::NetISP::LoadNetConfig() {}
/* 001d98c0 000000a4 */ EASO::NetISP::DnasConnect() {}
/* 001d9968 00000094 */ EASO::NetISP::DnasElfComplete() {}
/* 001d9a00 0000007c */ EASO::NetISP::DnasAuthDataComplete() {}
/* 001d9a80 000000f0 */ EASO::NetISP::DnasEncodedDataComplete() {}
/* 001d9b70 00000268 */ EASO::NetISP::DnasStart() {}
/* 001d9dd8 000000dc */ EASO::NetISP::CheckMTapStatus() {}
/* 001d9eb8 0000013c */ EASO::NetISP::SimulateDNASError() {}
/* 001d9ff8 00000088 */ EASO::NetISP::RestartSong() {}
/* 001da080 00000078 */ EASO::NetISP::CheckAdaptorStatus() {}
/* 001da0f8 000000b0 */ EASO::NetISP::CheckPPPStatus() {}
/* 001da1a8 00000008 */ EASO::NetISP::SetGameSpecificParams() {}
/* 001da1b0 00000068 */ EASO::NetISP::GetGameSpecificParams() {}
/* 001da218 00000114 */ EASO::NetISP::LaunchGame() {}
/* 001da330 000000c4 */ EASO::NetISP::CheckCDGameExist() {}
/* 001da3f8 00000100 */ EASO::NetISP::CheckEnoughMemoryMC() {}
/* 001da4f8 0000008c */ EASO::NetISP::CheckEnoughMemoryAlreadyDone() {}
/* 001da588 0000010c */ EASO::NetISP::CheckMCExist() {}
/* 001da698 000001c8 */ TranslateLanguage() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000001fc */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<char *>() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::ConstDbgIterator>() {}
/* ffffffff 00000078 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> & EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=<EASO::FifaStringPolicy>() {}
/* ffffffff 000000c4 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000148 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Reserve() {}
/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000060 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator=() {}
/* ffffffff 00000154 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Erase() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}
/* ffffffff 00000114 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Swap() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/lexical_cast.h
// *****************************************************************************

/* ffffffff 000001a4 */ EASO::String::LexicalCastNamespace::LexicalCastDetail::LexicalCast_To<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >::LexicalCast_From() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/EASOString.h
// *****************************************************************************

/* ffffffff 000001e4 */ void EASO::AddPair<int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<int>() {}
/* ffffffff 000001e4 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000001e4 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000002a8 */ EASO::UrlEncode() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/default_policy.h
// *****************************************************************************

/* 001d8090 0000012c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator EASO::String::DefaultPolicy::Copy<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/easoUtil.cpp
// *****************************************************************************

/* 001da888 0000015c */ EASO::EasoUtil::EasoUtil() {}
/* 001da9e8 00000028 */ EASO::EasoUtil::~EasoUtil() {}
/* 001daa10 0000006c */ EASO::EasoUtil::RegisterMethod() {}
/* 001daa80 00000154 */ EASO::EasoUtil::ExecuteMethod() {}
/* 001dabd8 000000a0 */ EASO::EasoUtil::GetXGameInvite() {}
/* 001dac78 00000148 */ EASO::EasoUtil::SetScoreParam() {}
/* 001dadc0 0000006c */ EASO::EasoUtil::SetDuplicateLogin() {}
/* 001dae30 0000021c */ EASO::EasoUtil::LoadDictionary() {}
/* 001db050 000001c0 */ EASO::EasoUtil::GetGIValue() {}
/* 001db210 000000bc */ EASO::EasoUtil::GetEnableDebug() {}
/* 001db2d0 000000f8 */ EASO::EasoUtil::SetLastOnlineState() {}
/* 001db3c8 00000064 */ EASO::EasoUtil::GetLastOnlineState() {}
/* 001db430 00000034 */ EASO::EasoUtil::isUTF8File() {}
/* 001db468 0000006c */ EASO::EasoUtil::AptGarbageCollect() {}
/* 001db4d8 00000084 */ EASO::EasoUtil::EnableAptCallFunctionTrace() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000001fc */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<char *>() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::ConstDbgIterator>() {}
/* ffffffff 00000078 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> & EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=<EASO::FifaStringPolicy>() {}
/* ffffffff 000000c4 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000148 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Reserve() {}
/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000060 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator=() {}
/* ffffffff 00000154 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Erase() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}
/* ffffffff 00000114 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Swap() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/lexical_cast.h
// *****************************************************************************

/* ffffffff 000001a4 */ EASO::String::LexicalCastNamespace::LexicalCastDetail::LexicalCast_To<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >::LexicalCast_From() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/EASOString.h
// *****************************************************************************

/* ffffffff 000001e4 */ void EASO::AddPair<int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<int>() {}
/* ffffffff 000001e4 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000002a8 */ EASO::UrlEncode() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/EasoUtil.h
// *****************************************************************************

/* ffffffff 00000008 */ EASO::EasoUtil::GetHomeUser() {}
/* ffffffff 00000008 */ EASO::EasoUtil::GetAwayUser() {}
/* ffffffff 00000008 */ EASO::EasoUtil::GetHomeScore() {}
/* ffffffff 00000008 */ EASO::EasoUtil::GetAwayScore() {}
/* ffffffff 00000008 */ EASO::EasoUtil::GetTID() {}
/* ffffffff 00000008 */ EASO::EasoUtil::GetTMID() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/default_policy.h
// *****************************************************************************

/* 001da860 0000012c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator EASO::String::DefaultPolicy::Copy<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/sfio/cfesharedlogininitialization.cpp
// *****************************************************************************

/* 001db588 00000014 */ static __tcf_1() {}
/* 001db5a0 000001a0 */ InitializeSharedLoginSingleton() {}
/* ffffffff 00000050 */ {anonymous}::CFESharedLoginAllocator::Instance() {}
/* ffffffff 00000030 */ {anonymous}::CFESharedLoginAllocator::~CFESharedLoginAllocator() {}
/* ffffffff 00000034 */ {anonymous}::CFESharedLoginAllocator::Alloc() {}
/* ffffffff 00000030 */ {anonymous}::CFESharedLoginAllocator::Free() {}
/* ffffffff 00000008 */ {anonymous}::CFESharedLoginAllocator::AddRef() {}
/* ffffffff 00000008 */ {anonymous}::CFESharedLoginAllocator::Release() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}

// *****************************************************************************
// FILE -- C:/work/dash/cfe/packages/Allocator/1.4.0/include/allocator/iallocator.h
// *****************************************************************************

/* 001db560 00000030 */ EA::Allocator::IAllocator::~IAllocator() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/netSettings.cpp
// *****************************************************************************

/* 001db768 00000114 */ EASO::NetSettings::NetSettings() {}
/* 001db880 00000058 */ EASO::NetSettings::~NetSettings() {}
/* 001db8d8 0000009c */ EASO::NetSettings::Abort() {}
/* 001db978 0000006c */ EASO::NetSettings::RegisterMethod() {}
/* 001db9e8 00000154 */ EASO::NetSettings::ExecuteMethod() {}
/* 001dbb40 0000002c */ EASO::NetSettings::SetLobbyApi() {}
/* 001dbb70 000000a0 */ EASO::NetSettings::Destroy() {}
/* 001dbc10 00000198 */ EASO::NetSettings::GetValue() {}
/* 001dbda8 0000014c */ EASO::NetSettings::SetValue() {}
/* 001dbef8 00000044 */ EASO::NetSettings::ConstructAlertFlag() {}
/* 001dbf40 0000025c */ EASO::NetSettings::GetAlertValues() {}
/* 001dc1a0 0000019c */ EASO::NetSettings::SetAlertValues() {}
/* 001dc340 00000108 */ EASO::NetSettings::LoadSettings() {}
/* 001dc448 00000108 */ EASO::NetSettings::SaveSettings() {}
/* 001dc550 000000e8 */ EASO::NetSettings::SettingsCallback() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 000001fc */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<char *>() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::ConstDbgIterator>() {}
/* ffffffff 00000078 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> & EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=<EASO::FifaStringPolicy>() {}
/* ffffffff 000000c4 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000148 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Reserve() {}
/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000154 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Erase() {}
/* ffffffff 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/lexical_cast.h
// *****************************************************************************

/* ffffffff 000001a4 */ EASO::String::LexicalCastNamespace::LexicalCastDetail::LexicalCast_To<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >::LexicalCast_From() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}
/* ffffffff 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/EASOString.h
// *****************************************************************************

/* ffffffff 000001e4 */ void EASO::AddPair<int>() {}
/* ffffffff 0000006c */ void EASO::AddPair<int>() {}
/* ffffffff 000001e4 */ void EASO::AddPair<char *>() {}
/* ffffffff 0000006c */ void EASO::AddPair<char *>() {}
/* ffffffff 000002a8 */ EASO::UrlEncode() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/default_policy.h
// *****************************************************************************

/* 001db740 0000012c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator EASO::String::DefaultPolicy::Copy<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/sfio/cfesharedloginsingleton.cpp
// *****************************************************************************

/* 001dc660 00000044 */ static __tcf_1() {}
/* 001dc6a8 00000054 */ EASO::SharedLoginSingleton::Instance() {}
/* 001dc700 00000054 */ EASO::SharedLoginSingleton::Initialize() {}
/* 001dc758 00000048 */ EASO::SharedLoginSingleton::CheckCard() {}
/* 001dc7a0 00000060 */ EASO::SharedLoginSingleton::GetPassword() {}
/* 001dc800 00000060 */ EASO::SharedLoginSingleton::GetPopupTitle() {}
/* 001dc860 00000070 */ EASO::SharedLoginSingleton::GetPopupButtonCount() {}
/* 001dc8d0 00000080 */ EASO::SharedLoginSingleton::GetPopupButtonLabel() {}
/* 001dc950 0000005c */ EASO::SharedLoginSingleton::GetPopupButtonResponseCode() {}
/* 001dc9b0 00000048 */ EASO::SharedLoginSingleton::GetPopupDefaultButtonIndex() {}
/* 001dc9f8 00000060 */ EASO::SharedLoginSingleton::GetUsername() {}
/* 001dca58 00000008 */ EASO::SharedLoginSingleton::IsFinished() {}
/* 001dca60 00000008 */ EASO::SharedLoginSingleton::IsPopupRequired() {}
/* 001dca68 00000008 */ EASO::SharedLoginSingleton::IsSucceeded() {}
/* 001dca70 0000003c */ EASO::SharedLoginSingleton::Load() {}
/* 001dcab0 00000008 */ EASO::SharedLoginSingleton::ResetPopupRequiredFlag() {}
/* 001dcab8 0000009c */ EASO::SharedLoginSingleton::Save() {}
/* 001dcb58 0000002c */ EASO::SharedLoginSingleton::SendUserResponse() {}
/* 001dcb88 00000074 */ EASO::SharedLoginSingleton::ShutDown() {}
/* 001dcc00 000001ac */ EASO::SharedLoginSingleton::Update() {}
/* 001dcdb0 00000034 */ EASO::SharedLoginSingleton::SharedLoginSingleton() {}
/* 001dcde8 0000002c */ EASO::SharedLoginSingleton::GetPreviousPopupTitle() {}
/* 001dce18 00000094 */ EASO::SharedLoginSingleton::Reset() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/ea/string/basic_string.h
// *****************************************************************************

/* ffffffff 00000064 */ bool EASO::String::operator==<EASO::String::EncodingUtf8, EASO::FifaStringPolicy, EASO::FifaStringPolicy>() {}
/* ffffffff 00000020 */ bool EASO::String::operator!=<EASO::String::EncodingUtf8, EASO::FifaStringPolicy, EASO::FifaStringPolicy>() {}
/* ffffffff 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString() {}
/* ffffffff 00000060 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator=() {}
/* ffffffff 00000114 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Swap() {}

// *****************************************************************************
// FILE -- C:/work/dash/cfe/packages/SharedLogin/00.02.09-realmemcard-2.01.03/include/common/sharedlogin/EacSharedLogin.h
// *****************************************************************************

/* ffffffff 0000000c */ EACSL::SharedLoginData::Reset() {}
/* 001dc638 00000054 */ EACSL::Message::Reset() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHandlers/statbookwrap.cpp
// *****************************************************************************

/* 001dced8 000000b0 */ StatbookWrapCreate() {}
/* 001dcf88 00000058 */ StatbookWrapDestroy() {}
/* 001dcfe0 000001d0 */ StatbookWrapUpdate() {}
/* 001dd1b0 000000d0 */ StatbookWrapGetStats() {}
/* 001dd280 0000009c */ StatbookWrapCancel() {}

// *****************************************************************************
// FILE -- C:/Work/dash/cfe/packages/easoApt/DASH_2006/source/easoHub.h
// *****************************************************************************

/* ffffffff 00000024 */ static __tcf_0() {}
/* ffffffff 00000054 */ EASO::EasoHub::Instance() {}
/* ffffffff 00000024 */ EASO::EasoHub::~EasoHub() {}
/* 001dceb0 00000024 */ EASO::EasoHub::EasoHub() {}

// *****************************************************************************
// FILE -- ../../../source/ps2/voip-e.c
// *****************************************************************************

/* 002011b0 0000011c */ static _VoipCallRpc2() {}
/* 002012d0 0000003c */ static _VoipCallRpc() {}
/* 00201310 000000c4 */ VoipStartup() {}
/* 002013d8 00000008 */ VoipGetRef() {}
/* 002013e0 00000050 */ VoipShutdown() {}
/* 00201430 00000058 */ VoipSetLocalUser() {}
/* 00201488 000000dc */ VoipConnect2() {}
/* 00201568 00000030 */ VoipDisconnect() {}
/* 00201598 0000007c */ VoipRemote() {}
/* 00201618 0000000c */ VoipLocal() {}
/* 00201628 00000034 */ VoipMicrophone() {}
/* 00201660 00000034 */ VoipSpeaker() {}
/* 00201698 00000160 */ VoipStatus2() {}
/* 002017f8 00000040 */ VoipControl() {}
/* 00201838 00000050 */ VoipGetProfileStat() {}
/* 00201888 0000000c */ VoipGetProfileTime() {}

// *****************************************************************************
// FILE -- ../../../../../contrib/ps2isp/source/ps2/ps2isp.c
// *****************************************************************************

/* 00201898 00000010 */ static _PS2IspSetAlert() {}
/* 002018a8 00000050 */ static _PS2IspReset() {}
/* 002018f8 00000038 */ static _PS2IspFilterText() {}
/* 00201930 0000007c */ static _PS2IspCheckMemcard() {}
/* 002019b0 0000003c */ static _PS2IspSyncMemcard() {}
/* 002019f0 00000034 */ static _PS2IspInitMemcardTracking() {}
/* 00201a28 00000070 */ static _PS2IspCheckMemcardAsync() {}
/* 00201a98 0000004c */ static _PS2IspHwSupported() {}
/* 00201ae8 000000d0 */ static _PS2IspConnect() {}
/* 00201bb8 00000224 */ static _PS2IspUpdate() {}
/* 00201de0 000000a8 */ PS2IspCreate() {}
/* 00201e88 00000044 */ PS2IspDestroy() {}
/* 00201ed0 00000008 */ PS2IspAutomatic() {}
/* 00201ed8 00000128 */ PS2IspParams() {}
/* 00202000 00000514 */ PS2IspSubmit() {}
/* 00202518 00000090 */ PS2IspAbort() {}
/* 002025a8 00000164 */ PS2IspAlert() {}
/* 00202710 00000018 */ PS2IspGetStatus() {}
/* 00202728 00000014 */ PS2IspSetContext() {}
/* 00202740 00000008 */ PS2IspGetContext() {}
/* 00202748 000000c0 */ PS2IspClearAlert() {}
/* 00202808 00000010 */ PS2IspGetAlertEnum() {}
/* 00202818 0000000c */ PS2IspGetUniqueID() {}

// *****************************************************************************
// FILE -- ../../../../../contrib/ps2isp/source/ps2/sloginread.c
// *****************************************************************************

/* 00202828 000000c4 */ static _SLoginFindTag() {}
/* 002028f0 00000048 */ static _SLoginExtractTag() {}
/* 00202938 00000034 */ static _SLoginCalcCRC32() {}
/* 00202970 00000090 */ static _SLoginCalcCRC32Str() {}
/* 00202a00 000000d0 */ static _SLoginExtract() {}
/* 00202ad0 00000070 */ static _SLoginCheckFile() {}
/* 00202b40 000000d0 */ static _SLoginLoad() {}
/* 00202c10 000001d0 */ SLoginRead() {}

// *****************************************************************************
// FILE -- ../../../../../contrib/dirtydnas/source/ps2/dirtydnas.c
// *****************************************************************************

/* 00202f48 000000d0 */ _CryptArc4Init() {}
/* 00203018 00000080 */ _CryptArc4Apply() {}
/* 00203098 00000090 */ DirtyDnasInit() {}
/* 00203128 00000030 */ DirtyDnasShrink() {}
/* 00203158 000000bc */ DirtyDnasDecode() {}
/* 00203218 00000074 */ DirtyDnasExecute() {}
/* 00203290 00000020 */ DirtyDnasShutdown() {}

// *****************************************************************************
// FILE -- ../../../../../contrib/dirtydnas/source/ps2/dnasreloc.h
// *****************************************************************************

/* 00202de0 00000134 */ _dnasimports() {}
/* 00202f18 0000002c */ _dnasexports() {}

// *****************************************************************************
// FILE -- ../../../source/ps2/dirtyelf.c
// *****************************************************************************

/* 002032b0 00000018 */ static _DirtyElfValidateHeader() {}
/* 002032c8 0000001c */ static _DirtyElfGetSectionByNum() {}
/* 002032e8 000000a8 */ static _DirtyElfGetSectionByName() {}
/* 00203390 00000040 */ static _DirtyElfGetSectionOffset() {}
/* 002033d0 00000050 */ static _DirtyElfCalcPatchTableSize() {}
/* 00203420 00000058 */ static _DirtyElfSetPatchTable() {}
/* 00203478 00000038 */ static _DirtyElfPatchAddr() {}
/* 002034b0 00000254 */ static _DirtyElfRelocateSection() {}
/* 00203708 0000006c */ DirtyElfInit() {}
/* 00203778 000000f8 */ DirtyElfParse() {}
/* 00203870 000000c4 */ DirtyElfRelocate() {}
/* 00203938 0000009c */ DirtyElfShrink() {}
/* 002039d8 00000024 */ DirtyElfDefault() {}
/* 00203a00 0000007c */ DirtyElfUnload() {}
/* 00203a80 00000080 */ DirtyElfGetSymbolAddrByIndex() {}
/* 00203b00 000000b4 */ DirtyElfGetSymbolAddrByName() {}

// *****************************************************************************
// FILE -- ../../../../../contrib/dirtygraph/source/dirtygif.c
// *****************************************************************************

/* 00203bb8 00000048 */ static _DirtyGifParseColorTable() {}
/* 00203c00 00000208 */ static _DirtyGifParseHeader() {}
/* 00203e08 00000028 */ static _DirtyGifResetDecoder() {}
/* 00203e30 00000124 */ static _DirtyGifInitDecoder() {}
/* 00203f58 00000070 */ static _DirtyGifGetByte() {}
/* 00203fc8 000000e0 */ static _DirtyGifGetNextCode() {}
/* 002040a8 0000015c */ static _DirtyGifUpdateDecoder() {}
/* 00204208 0000018c */ static _DirtyGifUpdateBitmap() {}
/* 00204398 0000001c */ DirtyGifParse() {}
/* 002043b8 00000088 */ DirtyGifDecodePalette() {}
/* 00204440 00000108 */ DirtyGifDecodeImage() {}

// *****************************************************************************
// FILE -- ../../../source/ps2/usbkey.c
// *****************************************************************************

/* 00204548 000000e4 */ static _USBKeyLoadModules() {}
/* 00204630 00000074 */ static _USBKeyShutdownModule() {}
/* 002046a8 0000007c */ static _USBKeyRequestInfo() {}
/* 00204728 000000b8 */ static _USBKeyCompleteInfo() {}
/* 002047e0 00000074 */ static _USBKeyGetReadKeyboard() {}
/* 00204858 00000080 */ static _USBKeyRequestRead() {}
/* 002048d8 0000008c */ static _USBKeyCompleteRead() {}
/* 00204968 00000008 */ static _USBKeyNullProcessCB() {}
/* 00204970 00000124 */ USBKeyInit() {}
/* 00204a98 00000050 */ USBKeyShutdown() {}
/* 00204ae8 0000000c */ USBKeyGetMaxConnect() {}
/* 00204af8 00000084 */ USBKeyInitKeyboard() {}
/* 00204b80 0000000c */ USBKeySetPollRate() {}
/* 00204b90 000000f0 */ USBKeyProcess() {}
/* 00204c80 0000003c */ USBKeyReadKeyboard() {}
/* 00204cc0 00000018 */ USBKeyGetKeyboardStatus() {}
/* 00204cd8 00000068 */ USBKeyHasKeyboard() {}

// *****************************************************************************
// FILE -- ../../../source/ps2/vkascii.c
// *****************************************************************************

/* 00204d40 00000054 */ static _USBKeyTranslateVKeyTo106() {}
/* 00204d98 00000078 */ static _USBKeyTranslateVkey() {}
/* 00204e10 0000008c */ static _USBKeyTranslateAscii() {}
/* 00204ea0 000000dc */ static _USBKeyGetModKeyState() {}
/* 00204f80 00000054 */ static _USBKeyGetLedKeyState() {}
/* 00204fd8 000000b8 */ USBKeyGetVkState() {}
/* 00205090 0000006c */ USBKeyTranslateVkAscii() {}

// *****************************************************************************
// FILE -- msifrpc.c
// *****************************************************************************

/* 00205100 000000e8 */ sceSifMInitRpc() {}
/* 002051e8 00000024 */ static _iopnotify() {}
/* 00205210 00000030 */ sceSifMExitRpc() {}
/* 00205240 000000a8 */ static _sceRpcGetPacket() {}
/* 002052e8 0000001c */ static _sceRpcFreePacket() {}
/* 00205308 00000120 */ static _request_end() {}
/* 00205428 00000024 */ sceSifMBindRpc() {}
/* 00205450 000001ac */ sceSifMBindRpcParam() {}
/* 00205600 0000011c */ sceSifMUnBindRpc() {}
/* 00205720 00000268 */ sceSifMCallRpc() {}

// *****************************************************************************
// FILE -- ../eecdvd.c
// *****************************************************************************

/* 00205988 000000b4 */ sceCdPowerOff() {}

// *****************************************************************************
// FILE -- ../eecdvd.c
// *****************************************************************************

/* 00205a40 000000ec */ sceCdTrayReq() {}

// *****************************************************************************
// FILE -- ../libmc.c
// *****************************************************************************

/* 00205b30 000001cc */ sceMcInit() {}
/* 00205d00 0000004c */ sceMcEnd() {}
/* 00205d50 00000030 */ _lmcGetClientPtr() {}
/* 00205d80 0000000c */ static mceSifRpcEndFunc() {}
/* 00205d90 000000bc */ sceMcChangeThreadPriority() {}
/* 00205e50 000000c4 */ sceMcGetSlotMax() {}
/* 00205f18 00000128 */ sceMcOpen() {}
/* 00206040 00000034 */ sceMcMkdir() {}
/* 00206078 000000bc */ sceMcClose() {}
/* 00206138 000000e0 */ sceMcSeek() {}
/* 00206218 00000098 */ static mceIntrReadFixAlign() {}
/* 002062b0 00000114 */ sceMcRead() {}
/* 002063c8 00000178 */ sceMcWrite() {}
/* 00206540 000000c4 */ sceMcSync() {}
/* 00206608 00000058 */ static mceGetInfoApdx() {}
/* 00206660 0000017c */ sceMcGetInfo() {}
/* 002067e0 00000070 */ sceMcUdCheckNewCard() {}
/* 00206850 0000014c */ sceMcGetDir() {}
/* 002069a0 0000008c */ static mceStorePwd() {}
/* 00206a30 00000140 */ sceMcChdir() {}
/* 00206b70 000000cc */ sceMcFormat() {}
/* 00206c40 0000011c */ sceMcDelete() {}
/* 00206d60 000000bc */ sceMcFlush() {}
/* 00206e20 000001cc */ sceMcSetFileInfo() {}
/* 00206ff0 0000015c */ sceMcRename() {}
/* 00207150 000000cc */ sceMcUnformat() {}
/* 00207220 0000010c */ sceMcGetEntSpace() {}

// *****************************************************************************
// FILE -- libusbkb.c
// *****************************************************************************

/* 00207330 000001f8 */ sceUsbKbInit() {}
/* 00207528 000000c8 */ sceUsbKbEnd() {}
/* 002075f0 000000a8 */ sceUsbKbGetInfo() {}
/* 00207698 000000dc */ sceUsbKbRead() {}
/* 00207778 000000dc */ sceUsbKbGetLocation() {}
/* 00207858 000000d8 */ sceUsbKbSetLEDStatus() {}
/* 00207930 000000fc */ sceUsbKbSetLEDMode() {}
/* 00207a30 00000044 */ sceUsbKbSetRepeat() {}
/* 00207a78 0000004c */ sceUsbKbSetCodeType() {}
/* 00207ac8 0000004c */ sceUsbKbSetArrangement() {}
/* 00207b18 000000a0 */ sceUsbKbSync() {}
/* 00207bb8 0000000c */ static rpccall_end() {}
/* 00207bc8 00000064 */ static SetResult() {}
/* 00207c30 00000058 */ static SetResult_GetInfo() {}
/* 00207c88 00000574 */ static SetResult_Read() {}
/* 00208200 0000004c */ static SetResult_GetLocation() {}
/* 00208250 00000020 */ static SetResult_SetLEDStatus() {}
/* 00208270 00000020 */ static SetResult_SetLEDMode() {}
/* 00208290 00000084 */ static vblank_e_handler() {}
/* 00208318 00000030 */ static clear_repbuf() {}
/* 00208348 0000006c */ static push_repbuf() {}
/* 002083b8 00000068 */ static pop_repbuf() {}
/* 00208420 0000014c */ static cnv_keycode() {}
/* 00208570 00000158 */ sceUsbKbCnvRawCode() {}
/* 002086c8 00000070 */ static cnv_keypad() {}
/* 00208738 0000006c */ sceUsbKbSetReadMode() {}
/* 002087a8 000000e8 */ sceUsbKbClearRbuf() {}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\ps2\ps2_interface.cpp
// *****************************************************************************

/* 00208890 00000030 */ void Realmc::SetPS2MessageMode(/* 0x0(sp) */ PS2MessageMode mode) {}
/* 002088c0 0000012c */ Interface* Realmc::Interface::CreateInstance(/* 0x0(sp) */ ConfigSettings &settings) {}

/* 002089f0 00000620 */ PS2Interface* Realmc::PS2Interface::PS2Interface(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ ConfigSettings &settings) {
	/* 0x4(sp) */ int err;
}

/* 00209010 00000110 */ void Realmc::PS2Interface::~PS2Interface(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ int __in_chrg) {}

/* 00209120 000001c8 */ char* Realmc::PS2Interface::PortStr(/* s0 16 */ PS2Interface *this, /* s1 17 */ CardID cardID) {
	/* bss 539598 */ static char pstr[16];
	/* 0x0(sp) */ __wchar_t *prefix;
	/* 0x4(sp) */ unsigned int lenPrefix;
}

/* 002092e8 000000b0 */ char* Realmc::PS2Interface::PortStrAscii(/* s0 16 */ PS2Interface *this, /* s1 17 */ CardID cardID) {
	/* bss 5395a8 */ static char pstr[4];
}

/* 00209398 000000d0 */ void Realmc::PS2Interface::CheckIopModule(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ char *moduleName) {
	/* 0x4(sp) */ int moduleId;
	/* 0x10(sp) */ char msg[512];
}

/* 00209468 000001dc */ void Realmc::PS2Interface::CheckName(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ char *str) {
	/* 0x4(sp) */ int i;
}

/* 00209648 000001d8 */ void Realmc::PS2Interface::CheckFileInfo(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ FileInfo *fileInfo) {}

/* 00209820 000000c4 */ bool Realmc::PS2Interface::CheckSyncResult(/* s0 16 */ PS2Interface *this) {
	/* 0x0(sp) */ bool busy;
	/* 0x4(sp) */ int cmd;
	/* 0x8(sp) */ int state;
}

/* 002098e8 0000021c */ void Realmc::PS2Interface::CheckSceResult(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ int sceFunctionResult) {}

/* 00209b08 00000694 */ CardStatus Realmc::PS2Interface::SceResultToStatus(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ int cmd, /* 0x4(sp) */ int result) {
	/* 0x8(sp) */ CardStatus status;
}

/* 0020a1a0 00000130 */ CardStatus Realmc::PS2Interface::CheckCard(/* s1 17 */ PS2Interface *this, /* s0 16 */ CardID cid, /* 0x0(sp) */ int *type, /* 0x4(sp) */ int *freeSpace, /* 0x8(sp) */ int *formatted) {
	/* 0xc(sp) */ int cType;
}

/* 0020a2d0 000000b0 */ bool Realmc::PS2Interface::MultitapConnected(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ int logicalport) {
	/* 0x4(sp) */ int physicalport;
}

/* 0020a380 000001c4 */ bool Realmc::PS2Interface::GetNextCardID(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &currentID) {
	/* 0x10(sp) */ CardID nextID;
	/* 0x20(sp) */ bool done;
	/* 0x24(sp) */ bool advanceSlot;
}

/* 0020a548 000000ec */ void Realmc::PS2Interface::SetFileTime(/* v0 2 */ PS2Interface *this, /* 0x0(sp) */ FileTime *destTime, /* 0x4(sp) */ sceMcTblGetDir *srcTime) {}

/* 0020a638 00000100 */ Ps2FileDescriptor* Realmc::PS2Interface::FindFreeFileDescriptor(/* s0 16 */ PS2Interface *this) {
	/* 0x0(sp) */ Ps2FileDescriptor *fd;
	/* 0x4(sp) */ int i;
}

/* 0020a738 00000108 */ Ps2FileDescriptor* Realmc::PS2Interface::SceOpenFile(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ char *fileName, /* 0x8(sp) */ int sceMode) {}
/* 0020a840 00000080 */ void Realmc::PS2Interface::SceCloseFile(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ Ps2FileDescriptor *fd) {}
/* 0020a8c0 00000054 */ void Realmc::PS2Interface::ClearTask(/* s0 16 */ PS2Interface *this) {}
/* 0020a918 000000d8 */ bool Realmc::PS2Interface::CheckCurrentTask(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ char *newTaskName) {}
/* 0020a9f0 000003ec */ void Realmc::PS2Interface::UpdateCurrentTask(/* s0 16 */ PS2Interface *this) {}
/* 0020ade0 00000074 */ static void __static_initialization_and_destruction_0(/* 0x0(sp) */ int __initialize_p, /* 0x4(sp) */ int __priority) {}
/* 0020ae58 00000038 */ CardID* Realmc::CardID::CardID(/* v1 3 */ CardID *this) {}
/* 0020ae90 00000050 */ CardID* Realmc::CardID::CardID(/* a3 7 */ CardID *this, /* a2 6 */ short unsigned int p, /* a1 5 */ short unsigned int s) {}
/* 0020aee0 00000060 */ void Realmc::MessageTimer::~MessageTimer(/* s0 16 */ MessageTimer *this, /* s1 17 */ int __in_chrg) {}
/* 0020af40 00000040 */ MessageTimer* Realmc::MessageTimer::MessageTimer(/* v1 3 */ MessageTimer *this) {}
/* 0020af80 0000002c */ void Realmc::MessageTimer::Init(/* v0 2 */ MessageTimer *this, /* v1 3 */ unsigned int nSecondsDefaultDelay) {}
/* 0020afb0 00000064 */ PS2Message* Realmc::PS2Message::PS2Message(/* s0 16 */ PS2Message *this) {}
/* 0020b018 0000006c */ void Realmc::PS2Message::~PS2Message(/* s1 17 */ PS2Message *this, /* s0 16 */ int __in_chrg) {}
/* 0020b088 00000040 */ void Realmc::PS2Message::Init(/* s0 16 */ PS2Message *this) {}
/* 0020b0c8 00000054 */ void Realmc::PS2Message::Clear(/* s0 16 */ PS2Message *this) {}
/* 0020b120 00000034 */ TaskID Realmc::Task::GetID(/* v1 3 */ Task *this) {}
/* 0020b158 00000064 */ TaskManager* Realmc::TaskManager::TaskManager(/* s0 16 */ TaskManager *this) {}
/* 0020b1c0 00000170 */ void Realmc::TaskManager::StartTask(/* s1 17 */ TaskManager *this, /* s0 16 */ Task *newTask) {}
/* 0020b330 000000dc */ void Realmc::TaskManager::EndTask(/* s1 17 */ TaskManager *this, /* s0 16 */ Task *task) {}
/* 0020b410 00000034 */ Task* Realmc::TaskManager::GetCurrentTask(/* v1 3 */ TaskManager *this) {}
/* 0020b448 00000080 */ void Realmc::TaskManager::ClearTask(/* s0 16 */ TaskManager *this) {}
/* 0020b4c8 00000064 */ Ps2FileDescriptor* Realmc::Ps2FileDescriptor::Ps2FileDescriptor(/* s0 16 */ Ps2FileDescriptor *this) {}
/* 0020b530 000000a0 */ TaskTrcStartGame* Realmc::TaskTrcStartGame::TaskTrcStartGame(/* s1 17 */ TaskTrcStartGame *this, /* s0 16 */ TaskManager *parent) {}
/* 0020b5d0 0000006c */ TaskTrcGetCardInfo* Realmc::TaskTrcGetCardInfo::TaskTrcGetCardInfo(/* s1 17 */ TaskTrcGetCardInfo *this, /* s0 16 */ TaskManager *parent) {}
/* 0020b640 000000b8 */ TaskTrcSaveFile* Realmc::TaskTrcSaveFile::TaskTrcSaveFile(/* s1 17 */ TaskTrcSaveFile *this, /* s0 16 */ TaskManager *parent) {}
/* 0020b6f8 000000b4 */ TaskTrcLoadFile* Realmc::TaskTrcLoadFile::TaskTrcLoadFile(/* s1 17 */ TaskTrcLoadFile *this, /* s0 16 */ TaskManager *parent) {}
/* 0020b7b0 000000a4 */ TaskTrcDeleteFile* Realmc::TaskTrcDeleteFile::TaskTrcDeleteFile(/* s1 17 */ TaskTrcDeleteFile *this, /* s0 16 */ TaskManager *parent) {}
/* 0020b858 00000098 */ TaskTrcListFiles* Realmc::TaskTrcListFiles::TaskTrcListFiles(/* s1 17 */ TaskTrcListFiles *this, /* s0 16 */ TaskManager *parent) {}
/* 0020b8f0 00000074 */ TaskStartGameCommon* Realmc::TaskStartGameCommon::TaskStartGameCommon(/* s1 17 */ TaskStartGameCommon *this, /* s0 16 */ TaskManager *parent) {}
/* 0020b968 00000080 */ TaskGetCardInfo* Realmc::TaskGetCardInfo::TaskGetCardInfo(/* s1 17 */ TaskGetCardInfo *this, /* s0 16 */ TaskManager *parent) {}
/* 0020b9e8 00000084 */ TaskCreateDirectory* Realmc::TaskCreateDirectory::TaskCreateDirectory(/* s1 17 */ TaskCreateDirectory *this, /* s0 16 */ TaskManager *parent) {}
/* 0020ba70 000000ac */ TaskOpen* Realmc::TaskOpen::TaskOpen(/* s1 17 */ TaskOpen *this, /* s0 16 */ TaskManager *parent) {}
/* 0020bb20 00000070 */ TaskClose* Realmc::TaskClose::TaskClose(/* s1 17 */ TaskClose *this, /* s0 16 */ TaskManager *parent) {}
/* 0020bb90 00000078 */ TaskSeek* Realmc::TaskSeek::TaskSeek(/* s1 17 */ TaskSeek *this, /* s0 16 */ TaskManager *parent) {}
/* 0020bc08 0000007c */ TaskRead* Realmc::TaskRead::TaskRead(/* s1 17 */ TaskRead *this, /* s0 16 */ TaskManager *parent) {}
/* 0020bc88 0000007c */ TaskWrite* Realmc::TaskWrite::TaskWrite(/* s1 17 */ TaskWrite *this, /* s0 16 */ TaskManager *parent) {}
/* 0020bd08 00000070 */ TaskFlush* Realmc::TaskFlush::TaskFlush(/* s1 17 */ TaskFlush *this, /* s0 16 */ TaskManager *parent) {}
/* 0020bd78 00000088 */ TaskDelete* Realmc::TaskDelete::TaskDelete(/* s1 17 */ TaskDelete *this, /* s0 16 */ TaskManager *parent) {}
/* 0020be00 0000006c */ TaskFormat* Realmc::TaskFormat::TaskFormat(/* s1 17 */ TaskFormat *this, /* s0 16 */ TaskManager *parent) {}
/* 0020be70 000000a0 */ TaskSetAttribute* Realmc::TaskSetAttribute::TaskSetAttribute(/* s1 17 */ TaskSetAttribute *this, /* s0 16 */ TaskManager *parent) {}
/* 0020bf10 00000088 */ TaskCheckSpace* Realmc::TaskCheckSpace::TaskCheckSpace(/* s1 17 */ TaskCheckSpace *this, /* s0 16 */ TaskManager *parent) {}
/* 0020bf98 000000ac */ TaskSaveFile* Realmc::TaskSaveFile::TaskSaveFile(/* s1 17 */ TaskSaveFile *this, /* s0 16 */ TaskManager *parent) {}
/* 0020c048 000000a4 */ TaskFind* Realmc::TaskFind::TaskFind(/* s1 17 */ TaskFind *this, /* s0 16 */ TaskManager *parent) {}
/* 0020c0f0 00000084 */ TaskCheckDirectory* Realmc::TaskCheckDirectory::TaskCheckDirectory(/* s1 17 */ TaskCheckDirectory *this, /* s0 16 */ TaskManager *parent) {}
/* 0020c178 00000084 */ TaskChangeDirectory* Realmc::TaskChangeDirectory::TaskChangeDirectory(/* s1 17 */ TaskChangeDirectory *this, /* s0 16 */ TaskManager *parent) {}
/* 0020c200 00000064 */ void* Realmc::PS2Interface::operator new(/* s0 16 */ size_t size) {}
/* 0020c268 00000050 */ void Realmc::PS2Interface::operator delete(/* s0 16 */ void *ptr, /* s1 17 */ size_t size) {}
/* 0020c2b8 00000064 */ void* Realmc::PS2Interface::operator new [](/* s0 16 */ size_t size) {}
/* 0020c320 00000050 */ void Realmc::PS2Interface::operator delete [](/* s0 16 */ void *ptr, /* s1 17 */ size_t size) {}
/* 0020c370 00000034 */ void* Realmc::PS2Interface::operator new(/* a0 4 */ void *ptr) {}
/* 0020c3a8 00000028 */ void Realmc::PS2Interface::operator delete() {}
/* 0020c3d0 00000034 */ void* Realmc::PS2Interface::operator new [](/* a0 4 */ void *ptr) {}
/* 0020c408 00000028 */ void Realmc::PS2Interface::operator delete []() {}
/* 0020c430 00000054 */ bool Realmc::PS2Interface::IsBusy(/* s0 16 */ PS2Interface *this) {}
/* 0020c488 00000034 */ Sjis* Realmc::PS2Interface::GetSjisInterface(/* v1 3 */ PS2Interface *this) {}
/* 0020c4c0 0000003c */ unsigned int Realmc::PS2Interface::GetBlockSize(/* v1 3 */ PS2Interface *this) {}
/* 0020c500 00000034 */ BlockCalculator* Realmc::PS2Interface::GetBlockCalculator(/* v1 3 */ PS2Interface *this) {}
/* 0020c538 0000003c */ FileInfo* Realmc::FileInfo::FileInfo(/* v1 3 */ FileInfo *this) {}
/* 0020c578 0000006c */ StartGameInfo* Realmc::StartGameInfo::StartGameInfo(/* s0 16 */ StartGameInfo *this) {}
/* 0020c5e8 00000058 */ Message* Realmc::Message::Message(/* s0 16 */ Message *this) {}
/* 0020c640 00000090 */ CmnFileDescriptor* Realmc::CmnFileDescriptor::CmnFileDescriptor(/* s0 16 */ CmnFileDescriptor *this) {}

/* 0020c6d0 0000004c */ TaskState Realmc::Task::SetState(/* a3 7 */ Task *this, /* v1 3 */ TaskState newState, /* a0 4 */ TaskState subState) {
	/* 0x0(sp) */ TaskState prevState;
}

/* 0020c720 00000034 */ TaskStack* Realmc::TaskStack::TaskStack(/* v1 3 */ TaskStack *this) {}
/* 0020c758 000000dc */ void Realmc::TaskStack::Push(/* s1 17 */ TaskStack *this, /* s0 16 */ Task *task) {}
/* 0020c838 00000068 */ void Realmc::TaskStack::Pop(/* v1 3 */ TaskStack *this, /* v0 2 */ Task **task) {}
/* 0020c8a0 0000008c */ void Realmc::TaskManager::UpdateCurrentTask(/* s0 16 */ TaskManager *this) {}
/* 0020c930 000000a4 */ bool Realmc::TaskManager::IsPublicTrcTask(/* s0 16 */ TaskManager *this, /* s1 17 */ Task *task) {}
/* 0020c9d8 000000cc */ void Realmc::TaskManager::TrcSingletonAssert(/* s1 17 */ TaskManager *this, /* s0 16 */ Task *newTask) {}
/* 0020caa8 00000054 */ void Realmc::Ps2FileDescriptor::Clear(/* s0 16 */ Ps2FileDescriptor *this) {}
/* 0020cb00 00000098 */ void Realmc::Ps2FileDescriptor::Init(/* s3 19 */ Ps2FileDescriptor *this, /* s0 16 */ CardID &cardID, /* s1 17 */ FileInfo &fileInfo, /* s2 18 */ FileOpenMode mode) {}
/* 0020cb98 00000098 */ Ps2Task* Realmc::Ps2Task::Ps2Task(/* s2 18 */ Ps2Task *this, /* s0 16 */ TaskID id, /* s1 17 */ TaskManager *parent) {}
/* 0020cc30 00000044 */ void Realmc::Ps2Task::Init(/* v1 3 */ Ps2Task *this, /* v0 2 */ bool notifyUser) {}
/* 0020cc78 00000090 */ void Realmc::TaskCheckSpace::Clear(/* s0 16 */ TaskCheckSpace *this) {}
/* 0020cd08 00000030 */ OpenFileDescriptor* Realmc::OpenFileDescriptor::OpenFileDescriptor(/* v1 3 */ OpenFileDescriptor *this) {}
/* 0020cd38 00000050 */ DetailInfo* Realmc::Message::DetailInfo::DetailInfo(/* s0 16 */ DetailInfo *this) {}
/* 0020cd88 00000058 */ void Realmc::CmnFileDescriptor::Clear(/* s0 16 */ CmnFileDescriptor *this) {}
/* 0020cde0 00000070 */ void Realmc::CmnFileDescriptor::Init(/* t0 8 */ CmnFileDescriptor *this, /* v0 2 */ CardID &cardID, /* v1 3 */ FileInfo &fileInfo, /* a0 4 */ FileOpenMode mode) {}
/* 0020ce50 00000054 */ Task* Realmc::Task::Task(/* a3 7 */ Task *this, /* v1 3 */ TaskID id, /* a0 4 */ TaskManager *parent) {}
/* 0020cea8 0000004c */ CardInfo* Realmc::Message::DetailInfo::CardInfo::CardInfo(/* s0 16 */ CardInfo *this) {}
/* 0020cef8 00000030 */ void Realmc::Task::Init(/* v1 3 */ Task *this) {}
/* 0020cf28 00000048 */ void _GLOBAL_$I$C__packages_realmemcard_2.02.03_source_lib_ps2_ps2_interface.cppL0aaaa() {}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\ps2\ps2_internaltasks.cpp
// *****************************************************************************

/* 0020cf70 00000aec */ void Realmc::PS2Interface::UpdateTaskStartGameCommon(/* s0 16 */ PS2Interface *this) {
	/* 0x10(sp) */ bool foundGoodCard;
	/* 0x18(sp) */ bool foundFullCard;
	/* 0x14(sp) */ int iPort;
	/* 0x18(sp) */ int iSlot;
	/* 0x1c(sp) */ int iPort;
	/* 0x20(sp) */ int iSlot;
}

/* 0020da60 00000e00 */ void Realmc::PS2Interface::UpdateTaskCheckSpace(/* s1 17 */ PS2Interface *this) {
	/* 0x10(sp) */ int tblsize;
	/* 0x14(sp) */ void *dirTable;
	/* 0x14(sp) */ CardStatus newStatus;
}

/* 0020e860 000003d0 */ void Realmc::PS2Interface::UpdateTaskSaveFile(/* s0 16 */ PS2Interface *this) {}
/* 0020ec30 000001d8 */ void Realmc::PS2Interface::UpdateTaskChangeDirectory(/* s0 16 */ PS2Interface *this) {}
/* 0020ee08 000004ac */ void Realmc::PS2Interface::UpdateTaskCheckDirectory(/* s0 16 */ PS2Interface *this) {}
/* 0020f2b8 00000044 */ TagValuePair* EA::TagValuePair::TagValuePair(/* a3 7 */ TagValuePair *this, /* v1 3 */ unsigned int tag, /* a0 4 */ int value) {}
/* 0020f300 00000044 */ TagValuePair* EA::TagValuePair::TagValuePair(/* a3 7 */ TagValuePair *this, /* v1 3 */ unsigned int tag, /* a0 4 */ void *value) {}
/* 0020f348 00000038 */ TagValuePair& EA::TagValuePair::operator+(/* v1 3 */ TagValuePair *this, /* a0 4 */ TagValuePair &rhs) {}
/* 0020f380 00000060 */ void Realmc::MessageTimer::Set(/* v1 3 */ MessageTimer *this, /* v0 2 */ unsigned int nSeconds) {}
/* 0020f3e0 0000008c */ bool Realmc::MessageTimer::IsExpired(/* s0 16 */ MessageTimer *this) {}
/* 0020f470 00000050 */ void Realmc::PS2Message::Set(/* s1 17 */ PS2Message *this, /* s0 16 */ LibMessage libMsg) {}
/* 0020f4c0 00000088 */ void Realmc::PS2Message::Set(/* s3 19 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s2 18 */ int trcMsg, /* s1 17 */ char *slot) {}
/* 0020f548 00000100 */ void Realmc::PS2Message::Set(/* s5 21 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s3 19 */ int trcMsg, /* s1 17 */ char *slot, /* s2 18 */ int freeSpace, /* s4 20 */ MSG_PROMPT prompt) {}
/* 0020f648 00000034 */ TaskState Realmc::Task::GetState(/* v1 3 */ Task *this) {}

/* 0020f680 0000004c */ TaskState Realmc::Task::SetState(/* a3 7 */ Task *this, /* v1 3 */ TaskState newState, /* a0 4 */ TaskState subState) {
	/* 0x0(sp) */ TaskState prevState;
}

/* 0020f6d0 00000034 */ int Realmc::Task::GetSubstate(/* v1 3 */ Task *this) {}
/* 0020f708 0000002c */ void Realmc::Task::SetSubstate(/* v0 2 */ Task *this, /* v1 3 */ TaskState sst) {}
/* 0020f738 00000068 */ void Realmc::Ps2Task::End(/* s0 16 */ Ps2Task *this) {}
/* 0020f7a0 00000078 */ void Realmc::TaskStartGameCommon::RecordCardState(/* a0 4 */ TaskStartGameCommon *this, /* a1 5 */ CardID id, /* v0 2 */ CardStatus status, /* v1 3 */ bool fileExists) {}
/* 0020f818 000000cc */ void Realmc::TaskGetCardInfo::Start(/* s3 19 */ TaskGetCardInfo *this, /* s0 16 */ CardID cardID, /* s1 17 */ TaskID proxTaskID, /* s2 18 */ bool notifyUser) {}
/* 0020f8e8 00000168 */ void Realmc::TaskCheckSpace::Start(/* s4 20 */ TaskCheckSpace *this, /* s0 16 */ CardID cardID, /* s3 19 */ StartGameInfo *startGameInfo, /* s1 17 */ unsigned int freeCardSpace, /* s2 18 */ bool notifyUser) {}
/* 0020fa50 0000012c */ void Realmc::TaskChangeDirectory::Start(/* s3 19 */ TaskChangeDirectory *this, /* s0 16 */ CardID cardID, /* s2 18 */ char *dirName, /* s1 17 */ bool notifyUser) {}
/* 0020fb80 00000028 */ void Realmc::MessageTimer::Stop(/* v0 2 */ MessageTimer *this) {}
/* 0020fba8 00000054 */ void Realmc::PS2Message::Clear(/* s0 16 */ PS2Message *this) {}
/* 0020fc00 00000044 */ void Realmc::Ps2Task::Init(/* v1 3 */ Ps2Task *this, /* v0 2 */ bool notifyUser) {}
/* 0020fc48 00000090 */ void Realmc::TaskCheckSpace::Clear(/* s0 16 */ TaskCheckSpace *this) {}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\ps2\ps2_public.cpp
// *****************************************************************************

/* 0020fcd8 000000e4 */ Message* Realmc::PS2Interface::GetMessage(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ int elapsedTime) {}
/* 0020fdc0 000000c8 */ void Realmc::PS2Interface::SendMessage(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ UserMessage msg, /* 0x4(sp) */ int value) {}

/* 0020fe88 00000104 */ __wchar_t* Realmc::PS2Interface::GetCardName(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &id) {
	/* 0x4(sp) */ __wchar_t *p;
	/* 0x20(sp) */ int len;
}

/* 0020ff90 0000012c */ void Realmc::PS2Interface::TrcStartGame(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ StartGameInfo &info) {}
/* 002100c0 000000c0 */ void Realmc::PS2Interface::TrcCardExists(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID) {}
/* 00210180 000000c0 */ void Realmc::PS2Interface::TrcGetCardInfo(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID) {}
/* 00210240 00000110 */ void Realmc::PS2Interface::TrcLoadFile(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ FileInfo &fileInfo) {}

/* 00210350 00000230 */ void Realmc::PS2Interface::TrcSaveFile(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ FileInfo &fileInfo) {
	/* 0x8(sp) */ sceMcIconSys *pIconSys;
}

/* 00210580 00000110 */ void Realmc::PS2Interface::TrcDeleteFile(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ FileInfo &fileInfo) {}
/* 00210690 00000110 */ void Realmc::PS2Interface::TrcListFiles(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ FileInfo &fileInfo) {}
/* 002107a0 00000098 */ void Realmc::PS2Interface::CardExists(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID) {}
/* 00210838 00000098 */ void Realmc::PS2Interface::GetCardInfo(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID) {}
/* 002108d0 00000084 */ void Realmc::PS2Interface::Read(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ FileHandle fileHandle, /* 0x4(sp) */ void *buffer, /* 0x8(sp) */ int bufferSize) {}
/* 00210958 00000084 */ void Realmc::PS2Interface::Write(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ FileHandle fileHandle, /* 0x4(sp) */ void *buffer, /* 0x8(sp) */ int bufferSize) {}
/* 002109e0 00000088 */ void Realmc::PS2Interface::Seek(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ FileHandle fileHandle, /* 0x4(sp) */ int offset, /* 0x8(sp) */ SeekFrom seekFrom) {}
/* 00210a68 00000078 */ void Realmc::PS2Interface::Flush(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ FileHandle fileHandle) {}
/* 00210ae0 0000009c */ void Realmc::PS2Interface::CreateDirectory(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ DirNameType *directoryName) {}
/* 00210b80 000000a0 */ void Realmc::PS2Interface::DeleteDirectory(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ DirNameType *directoryName) {}
/* 00210c20 000000b4 */ void Realmc::PS2Interface::OpenFile(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ FileInfo &fileInfo, /* 0x8(sp) */ FileOpenMode mode) {}
/* 00210cd8 00000078 */ void Realmc::PS2Interface::CloseFile(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ FileHandle fileHandle) {}
/* 00210d50 000000a4 */ void Realmc::PS2Interface::DeleteFile(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ DirNameType *directoryName, /* 0x8(sp) */ char *fileName) {}
/* 00210df8 000000ac */ void Realmc::PS2Interface::SetFileAttribute(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ DirNameType *directoryName, /* 0x8(sp) */ char *fileName, /* 0xc(sp) */ FileAttribute attribute) {}
/* 00210ea8 000000a4 */ void Realmc::PS2Interface::FindFile(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ DirNameType *directoryName, /* 0x8(sp) */ char *fileName) {}
/* 00210f50 00000094 */ void Realmc::PS2Interface::Format(/* s0 16 */ PS2Interface *this, /* 0x0(sp) */ CardID &cardID) {}
/* 00210fe8 00000028 */ void Realmc::MessageTimer::Stop(/* v0 2 */ MessageTimer *this) {}
/* 00211010 00000040 */ void Realmc::MessageTimer::AddElapsedTime(/* v1 3 */ MessageTimer *this, /* v0 2 */ unsigned int elapsedTime) {}
/* 00211050 00000054 */ void Realmc::PS2Message::Clear(/* s0 16 */ PS2Message *this) {}
/* 002110a8 00000110 */ void Realmc::TaskTrcStartGame::Start(/* s3 19 */ TaskTrcStartGame *this, /* s0 16 */ CardID cID, /* s2 18 */ StartGameInfo *info, /* s1 17 */ int spaceNeeded) {}
/* 002111b8 000000ac */ void Realmc::TaskTrcGetCardInfo::Start(/* s2 18 */ TaskTrcGetCardInfo *this, /* s0 16 */ CardID &cID, /* s1 17 */ TaskID taskID) {}
/* 00211268 000000ec */ void Realmc::TaskTrcSaveFile::Start(/* s2 18 */ TaskTrcSaveFile *this, /* s0 16 */ CardID cID, /* s1 17 */ FileInfo *finfo) {}
/* 00211358 000000ec */ void Realmc::TaskTrcLoadFile::Start(/* s2 18 */ TaskTrcLoadFile *this, /* s0 16 */ CardID cID, /* s1 17 */ FileInfo *finfo) {}
/* 00211448 000000d8 */ void Realmc::TaskTrcDeleteFile::Start(/* s2 18 */ TaskTrcDeleteFile *this, /* s0 16 */ CardID cID, /* s1 17 */ FileInfo *finfo) {}
/* 00211520 000000cc */ void Realmc::TaskTrcListFiles::Start(/* s2 18 */ TaskTrcListFiles *this, /* s0 16 */ CardID cID, /* s1 17 */ FileInfo *finfo) {}
/* 002115f0 000000cc */ void Realmc::TaskGetCardInfo::Start(/* s3 19 */ TaskGetCardInfo *this, /* s0 16 */ CardID cardID, /* s1 17 */ TaskID proxTaskID, /* s2 18 */ bool notifyUser) {}
/* 002116c0 0000012c */ void Realmc::TaskCreateDirectory::Start(/* s3 19 */ TaskCreateDirectory *this, /* s0 16 */ CardID cardID, /* s2 18 */ char *dirName, /* s1 17 */ bool notifyUser) {}
/* 002117f0 00000108 */ void Realmc::TaskOpen::Start(/* s5 21 */ TaskOpen *this, /* s0 16 */ CardID cardID, /* s1 17 */ char *dirName, /* s2 18 */ char *fileName, /* s3 19 */ FileOpenMode mode, /* s4 20 */ bool notifyUser) {}
/* 002118f8 000000a0 */ void Realmc::TaskClose::Start(/* s2 18 */ TaskClose *this, /* s0 16 */ FileHandle fh, /* s1 17 */ bool notifyUser) {}
/* 00211998 000000c0 */ void Realmc::TaskSeek::Start(/* s4 20 */ TaskSeek *this, /* s0 16 */ FileHandle fh, /* s1 17 */ int offset, /* s2 18 */ SeekFrom from, /* s3 19 */ bool notifyUser) {}
/* 00211a58 000000b8 */ void Realmc::TaskRead::Start(/* s3 19 */ TaskRead *this, /* s0 16 */ FileHandle fh, /* s1 17 */ void *buffer, /* s2 18 */ int bufsize) {}
/* 00211b10 000000b8 */ void Realmc::TaskWrite::Start(/* s3 19 */ TaskWrite *this, /* s0 16 */ FileHandle fh, /* s1 17 */ void *buffer, /* s2 18 */ int bufsize) {}
/* 00211bc8 000000a0 */ void Realmc::TaskFlush::Start(/* s2 18 */ TaskFlush *this, /* s0 16 */ FileHandle fh, /* s1 17 */ bool notifyUser) {}
/* 00211c68 00000194 */ void Realmc::TaskDelete::Start(/* s4 20 */ TaskDelete *this, /* s0 16 */ CardID cardID, /* s2 18 */ char *dirName, /* s3 19 */ char *fileName, /* s1 17 */ bool notifyUser) {}
/* 00211e00 000000a8 */ void Realmc::TaskFormat::Start(/* s2 18 */ TaskFormat *this, /* s0 16 */ CardID cID, /* s1 17 */ bool notifyUser) {}
/* 00211ea8 00000100 */ void Realmc::TaskSetAttribute::Start(/* s5 21 */ TaskSetAttribute *this, /* s0 16 */ CardID cardID, /* s1 17 */ char *dirName, /* s2 18 */ char *fileName, /* s3 19 */ FileAttribute attr, /* s4 20 */ bool notifyUser) {}
/* 00211fa8 000000f8 */ void Realmc::TaskFind::Start(/* s4 20 */ TaskFind *this, /* s0 16 */ CardID cardID, /* s1 17 */ char *dirName, /* s2 18 */ char *fileName, /* s3 19 */ bool notifyUser) {}
/* 002120a0 00000044 */ void Realmc::Ps2Task::Init(/* v1 3 */ Ps2Task *this, /* v0 2 */ bool notifyUser) {}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\ps2\ps2_sjis.cpp
// *****************************************************************************

/* 002120e8 000001c0 */ unsigned char Realmc::Sjis::Sjis2Ascii(/* v1 3 */ Sjis *this, /* 0x0(sp) */ short unsigned int sjis) {
	/* 0x2(sp) */ unsigned char byte1;
	/* 0x3(sp) */ unsigned char byte2;
	/* 0x4(sp) */ int i;
}

/* 002122a8 00000228 */ short unsigned int Realmc::Sjis::Ascii2Sjis(/* v1 3 */ Sjis *this, /* 0x0(sp) */ unsigned char asciiCode) {
	/* 0x2(sp) */ short unsigned int sjisCode;
	/* 0x4(sp) */ unsigned char stmp;
	/* 0x5(sp) */ unsigned char stmp2;
}

/* 002124d0 000000c0 */ CharType Realmc::Sjis::CheckChar(/* v1 3 */ Sjis *this, /* 0x0(sp) */ unsigned char c) {}

/* 00212590 00000108 */ void Realmc::Sjis::Sjis2AsciiString(/* s0 16 */ Sjis *this, /* 0x0(sp) */ short unsigned int *sjisStr, /* 0x4(sp) */ unsigned char *asciiStr) {
	/* 0x8(sp) */ int i;
	/* 0xc(sp) */ int length;
	/* 0x10(sp) */ char temp;
}

/* 00212698 000000f0 */ void Realmc::Sjis::Ascii2SjisString(/* s0 16 */ Sjis *this, /* 0x0(sp) */ unsigned char *asciiStr, /* 0x4(sp) */ short unsigned int *sjisStr) {
	/* 0x8(sp) */ int i;
	/* 0xc(sp) */ int len;
	/* 0x10(sp) */ short unsigned int sjis;
}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\ps2\ps2_tasks.cpp
// *****************************************************************************

/* 00212788 00000114 */ Ps2FileDescriptor* Realmc::ConvertFileHandleToDescriptor(/* 0x0(sp) */ FileHandle h) {
	/* 0x4(sp) */ Ps2FileDescriptor *fd;
}

/* 002128a0 00000378 */ void Realmc::PS2Interface::UpdateTaskGetCardInfo(/* s0 16 */ PS2Interface *this) {}
/* 00212c18 00000268 */ void Realmc::PS2Interface::UpdateTaskWrite(/* s0 16 */ PS2Interface *this) {}
/* 00212e80 00000268 */ void Realmc::PS2Interface::UpdateTaskRead(/* s0 16 */ PS2Interface *this) {}
/* 002130e8 00000214 */ void Realmc::PS2Interface::UpdateTaskCreateDirectory(/* s0 16 */ PS2Interface *this) {}

/* 00213300 000002fc */ void Realmc::PS2Interface::UpdateTaskOpen(/* s0 16 */ PS2Interface *this) {
	/* 0x0(sp) */ char path[128];
	/* 0x80(sp) */ int sceMode;
}

/* 00213600 000001e4 */ void Realmc::PS2Interface::UpdateTaskClose(/* s0 16 */ PS2Interface *this) {}
/* 002137e8 00000180 */ void Realmc::PS2Interface::UpdateTaskFlush(/* s0 16 */ PS2Interface *this) {}
/* 00213968 000001a4 */ void Realmc::PS2Interface::UpdateTaskSeek(/* s0 16 */ PS2Interface *this) {}

/* 00213b10 00000244 */ void Realmc::PS2Interface::UpdateTaskSetAttribute(/* s0 16 */ PS2Interface *this) {
	/* 0x0(sp) */ sceMcTblGetDir info;
	/* 0x40(sp) */ char path[64];
}

/* 00213d58 00000144 */ void Realmc::PS2Interface::UpdateTaskDelete(/* s0 16 */ PS2Interface *this) {}
/* 00213ea0 00000354 */ void Realmc::PS2Interface::UpdateTaskFind(/* s0 16 */ PS2Interface *this) {}
/* 002141f8 00000138 */ void Realmc::PS2Interface::UpdateTaskFormat(/* s0 16 */ PS2Interface *this) {}
/* 00214330 00000044 */ bool Realmc::CmnFileDescriptor::CheckSignature(/* v1 3 */ CmnFileDescriptor *this) {}
/* 00214378 00000050 */ void Realmc::PS2Message::Set(/* s1 17 */ PS2Message *this, /* s0 16 */ LibMessage libMsg) {}
/* 002143c8 00000060 */ void Realmc::PS2Message::Set(/* s2 18 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s1 17 */ CardStatus _status) {}
/* 00214428 00000034 */ TaskState Realmc::Task::GetState(/* v1 3 */ Task *this) {}

/* 00214460 0000004c */ TaskState Realmc::Task::SetState(/* a3 7 */ Task *this, /* v1 3 */ TaskState newState, /* a0 4 */ TaskState subState) {
	/* 0x0(sp) */ TaskState prevState;
}

/* 002144b0 00000068 */ void Realmc::Ps2Task::End(/* s0 16 */ Ps2Task *this) {}
/* 00214518 0000012c */ void Realmc::TaskChangeDirectory::Start(/* s3 19 */ TaskChangeDirectory *this, /* s0 16 */ CardID cardID, /* s2 18 */ char *dirName, /* s1 17 */ bool notifyUser) {}
/* 00214648 00000054 */ void Realmc::PS2Message::Clear(/* s0 16 */ PS2Message *this) {}
/* 002146a0 00000044 */ void Realmc::Ps2Task::Init(/* v1 3 */ Ps2Task *this, /* v0 2 */ bool notifyUser) {}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\ps2\ps2_trctasks.cpp
// *****************************************************************************

/* 002146e8 00000140 */ void Realmc::PS2Interface::UpdateTaskTrcStartGame(/* s0 16 */ PS2Interface *this) {}

/* 00214828 000004f8 */ void Realmc::PS2Interface::UpdateTaskTrcStartGameAutoSave(/* s0 16 */ PS2Interface *this) {
	/* 0x10(sp) */ bool foundFile;
	/* 0x14(sp) */ int iPort;
	/* 0x18(sp) */ int iSlot;
}

/* 00214d20 000002b4 */ void Realmc::PS2Interface::UpdateTaskTrcGetCardInfo(/* s0 16 */ PS2Interface *this) {}

/* 00214fd8 00001d08 */ void Realmc::PS2Interface::UpdateTaskTrcSaveFile(/* s0 16 */ PS2Interface *this) {
	/* 0x10(sp) */ char path[64];
	/* 0x50(sp) */ int sceFileMode;
}

/* 00216ce0 0000139c */ void Realmc::PS2Interface::UpdateTaskTrcLoadFile(/* s0 16 */ PS2Interface *this) {}

/* 00218080 00000f84 */ void Realmc::PS2Interface::UpdateTaskTrcDeleteFile(/* s0 16 */ PS2Interface *this) {
	/* 0x10(sp) */ char rootpath[64];
}

/* 00219008 000007a0 */ void Realmc::PS2Interface::UpdateTaskTrcListFiles(/* s0 16 */ PS2Interface *this) {}
/* 002197a8 00000060 */ void Realmc::MessageTimer::Set(/* v1 3 */ MessageTimer *this, /* v0 2 */ unsigned int nSeconds) {}
/* 00219808 00000028 */ void Realmc::MessageTimer::Stop(/* v0 2 */ MessageTimer *this) {}
/* 00219830 0000008c */ bool Realmc::MessageTimer::IsExpired(/* s0 16 */ MessageTimer *this) {}
/* 002198c0 00000050 */ void Realmc::PS2Message::Set(/* s1 17 */ PS2Message *this, /* s0 16 */ LibMessage libMsg) {}
/* 00219910 00000060 */ void Realmc::PS2Message::Set(/* s2 18 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s1 17 */ CardStatus _status) {}
/* 00219970 00000074 */ void Realmc::PS2Message::Set(/* s2 18 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s1 17 */ int trcMsg) {}
/* 002199e8 000000dc */ void Realmc::PS2Message::Set(/* s3 19 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s1 17 */ int trcMsg, /* s2 18 */ MSG_PROMPT prompt) {}
/* 00219ac8 00000088 */ void Realmc::PS2Message::Set(/* s3 19 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s2 18 */ int trcMsg, /* s1 17 */ char *slot) {}
/* 00219b50 000000f0 */ void Realmc::PS2Message::Set(/* s4 20 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s2 18 */ int trcMsg, /* s1 17 */ char *slot, /* s3 19 */ MSG_PROMPT prompt) {}
/* 00219c40 00000100 */ void Realmc::PS2Message::Set(/* s5 21 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s3 19 */ int trcMsg, /* s1 17 */ char *content, /* s2 18 */ char *slot, /* s4 20 */ MSG_PROMPT prompt) {}
/* 00219d40 00000100 */ void Realmc::PS2Message::Set(/* s5 21 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s3 19 */ int trcMsg, /* s1 17 */ char *slot, /* s2 18 */ int freeSpace, /* s4 20 */ MSG_PROMPT prompt) {}
/* 00219e40 00000034 */ TaskState Realmc::Task::GetState(/* v1 3 */ Task *this) {}

/* 00219e78 0000004c */ TaskState Realmc::Task::SetState(/* a3 7 */ Task *this, /* v1 3 */ TaskState newState, /* a0 4 */ TaskState subState) {
	/* 0x0(sp) */ TaskState prevState;
}

/* 00219ec8 00000034 */ int Realmc::Task::GetSubstate(/* v1 3 */ Task *this) {}
/* 00219f00 0000002c */ void Realmc::Task::SetSubstate(/* v0 2 */ Task *this, /* v1 3 */ TaskState sst) {}
/* 00219f30 00000068 */ void Realmc::Ps2Task::End(/* s0 16 */ Ps2Task *this) {}
/* 00219f98 000000e4 */ void Realmc::TaskStartGameCommon::Start(/* s2 18 */ TaskStartGameCommon *this, /* s0 16 */ CardID cID, /* s1 17 */ int spaceNeeded) {}
/* 0021a080 000000cc */ void Realmc::TaskGetCardInfo::Start(/* s3 19 */ TaskGetCardInfo *this, /* s0 16 */ CardID cardID, /* s1 17 */ TaskID proxTaskID, /* s2 18 */ bool notifyUser) {}
/* 0021a150 000000b8 */ void Realmc::TaskRead::Start(/* s3 19 */ TaskRead *this, /* s0 16 */ FileHandle fh, /* s1 17 */ void *buffer, /* s2 18 */ int bufsize) {}
/* 0021a208 000000a8 */ void Realmc::TaskFormat::Start(/* s2 18 */ TaskFormat *this, /* s0 16 */ CardID cID, /* s1 17 */ bool notifyUser) {}

/* 0021a2b0 00000288 */ void Realmc::TaskCheckSpace::Start(/* s5 21 */ TaskCheckSpace *this, /* s3 19 */ CardID cardID, /* s4 20 */ FileInfo *fileInfo, /* s1 17 */ unsigned int freeCardSpace, /* s2 18 */ bool notifyUser) {
	/* 0x0(sp) */ BlockCalculatorImp *calculator;
}

/* 0021a538 0000010c */ void Realmc::TaskSaveFile::Start(/* s5 21 */ TaskSaveFile *this, /* s0 16 */ CardID cID, /* s1 17 */ char *dirName, /* s2 18 */ char *fname, /* s3 19 */ void *data, /* s4 20 */ int dataSize) {}
/* 0021a648 000000f8 */ void Realmc::TaskFind::Start(/* s4 20 */ TaskFind *this, /* s0 16 */ CardID cardID, /* s1 17 */ char *dirName, /* s2 18 */ char *fileName, /* s3 19 */ bool notifyUser) {}
/* 0021a740 00000120 */ void Realmc::TaskCheckDirectory::Start(/* s2 18 */ TaskCheckDirectory *this, /* s0 16 */ CardID cardID, /* s1 17 */ char *dirName) {}
/* 0021a860 0000012c */ void Realmc::TaskChangeDirectory::Start(/* s3 19 */ TaskChangeDirectory *this, /* s0 16 */ CardID cardID, /* s2 18 */ char *dirName, /* s1 17 */ bool notifyUser) {}
/* 0021a990 00000054 */ void Realmc::PS2Message::Clear(/* s0 16 */ PS2Message *this) {}
/* 0021a9e8 00000044 */ void Realmc::Ps2Task::Init(/* v1 3 */ Ps2Task *this, /* v0 2 */ bool notifyUser) {}
/* 0021aa30 00000090 */ void Realmc::TaskCheckSpace::Clear(/* s0 16 */ TaskCheckSpace *this) {}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\ps2\ps2_trctasks_nodelay.cpp
// *****************************************************************************

/* 0021aac0 00000140 */ void Realmc::PS2Interface::UpdateTaskTrcStartGameNoDelay(/* s0 16 */ PS2Interface *this) {}

/* 0021ac00 000004f8 */ void Realmc::PS2Interface::UpdateTaskTrcStartGameAutoSaveNoDelay(/* s0 16 */ PS2Interface *this) {
	/* 0x10(sp) */ bool foundFile;
	/* 0x14(sp) */ int iPort;
	/* 0x18(sp) */ int iSlot;
}

/* 0021b0f8 000002c0 */ void Realmc::PS2Interface::UpdateTaskTrcGetCardInfoNoDelay(/* s0 16 */ PS2Interface *this) {}

/* 0021b3b8 00001de4 */ void Realmc::PS2Interface::UpdateTaskTrcSaveFileNoDelay(/* s0 16 */ PS2Interface *this) {
	/* 0x10(sp) */ char path[64];
	/* 0x50(sp) */ int sceFileMode;
}

/* 0021d1a0 000013a0 */ void Realmc::PS2Interface::UpdateTaskTrcLoadFileNoDelay(/* s0 16 */ PS2Interface *this) {}

/* 0021e540 00000f98 */ void Realmc::PS2Interface::UpdateTaskTrcDeleteFileNoDelay(/* s0 16 */ PS2Interface *this) {
	/* 0x10(sp) */ char rootpath[64];
}

/* 0021f4d8 0000080c */ void Realmc::PS2Interface::UpdateTaskTrcListFilesNoDelay(/* s0 16 */ PS2Interface *this) {}
/* 0021fce8 00000060 */ void Realmc::MessageTimer::Set(/* v1 3 */ MessageTimer *this, /* v0 2 */ unsigned int nSeconds) {}
/* 0021fd48 0000008c */ bool Realmc::MessageTimer::IsExpired(/* s0 16 */ MessageTimer *this) {}
/* 0021fdd8 00000050 */ void Realmc::PS2Message::Set(/* s1 17 */ PS2Message *this, /* s0 16 */ LibMessage libMsg) {}
/* 0021fe28 00000060 */ void Realmc::PS2Message::Set(/* s2 18 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s1 17 */ CardStatus _status) {}
/* 0021fe88 000000dc */ void Realmc::PS2Message::Set(/* s3 19 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s1 17 */ int trcMsg, /* s2 18 */ MSG_PROMPT prompt) {}
/* 0021ff68 00000088 */ void Realmc::PS2Message::Set(/* s3 19 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s2 18 */ int trcMsg, /* s1 17 */ char *slot) {}
/* 0021fff0 000000f0 */ void Realmc::PS2Message::Set(/* s4 20 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s2 18 */ int trcMsg, /* s1 17 */ char *slot, /* s3 19 */ MSG_PROMPT prompt) {}
/* 002200e0 00000100 */ void Realmc::PS2Message::Set(/* s5 21 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s3 19 */ int trcMsg, /* s1 17 */ char *content, /* s2 18 */ char *slot, /* s4 20 */ MSG_PROMPT prompt) {}
/* 002201e0 00000100 */ void Realmc::PS2Message::Set(/* s5 21 */ PS2Message *this, /* s0 16 */ LibMessage libMsg, /* s3 19 */ int trcMsg, /* s1 17 */ char *slot, /* s2 18 */ int freeSpace, /* s4 20 */ MSG_PROMPT prompt) {}
/* 002202e0 00000034 */ TaskState Realmc::Task::GetState(/* v1 3 */ Task *this) {}

/* 00220318 0000004c */ TaskState Realmc::Task::SetState(/* a3 7 */ Task *this, /* v1 3 */ TaskState newState, /* a0 4 */ TaskState subState) {
	/* 0x0(sp) */ TaskState prevState;
}

/* 00220368 00000034 */ int Realmc::Task::GetSubstate(/* v1 3 */ Task *this) {}
/* 002203a0 0000002c */ void Realmc::Task::SetSubstate(/* v0 2 */ Task *this, /* v1 3 */ TaskState sst) {}
/* 002203d0 00000068 */ void Realmc::Ps2Task::End(/* s0 16 */ Ps2Task *this) {}
/* 00220438 000000e4 */ void Realmc::TaskStartGameCommon::Start(/* s2 18 */ TaskStartGameCommon *this, /* s0 16 */ CardID cID, /* s1 17 */ int spaceNeeded) {}
/* 00220520 000000cc */ void Realmc::TaskGetCardInfo::Start(/* s3 19 */ TaskGetCardInfo *this, /* s0 16 */ CardID cardID, /* s1 17 */ TaskID proxTaskID, /* s2 18 */ bool notifyUser) {}
/* 002205f0 000000b8 */ void Realmc::TaskRead::Start(/* s3 19 */ TaskRead *this, /* s0 16 */ FileHandle fh, /* s1 17 */ void *buffer, /* s2 18 */ int bufsize) {}
/* 002206a8 000000a8 */ void Realmc::TaskFormat::Start(/* s2 18 */ TaskFormat *this, /* s0 16 */ CardID cID, /* s1 17 */ bool notifyUser) {}

/* 00220750 00000288 */ void Realmc::TaskCheckSpace::Start(/* s5 21 */ TaskCheckSpace *this, /* s3 19 */ CardID cardID, /* s4 20 */ FileInfo *fileInfo, /* s1 17 */ unsigned int freeCardSpace, /* s2 18 */ bool notifyUser) {
	/* 0x0(sp) */ BlockCalculatorImp *calculator;
}

/* 002209d8 0000010c */ void Realmc::TaskSaveFile::Start(/* s5 21 */ TaskSaveFile *this, /* s0 16 */ CardID cID, /* s1 17 */ char *dirName, /* s2 18 */ char *fname, /* s3 19 */ void *data, /* s4 20 */ int dataSize) {}
/* 00220ae8 000000f8 */ void Realmc::TaskFind::Start(/* s4 20 */ TaskFind *this, /* s0 16 */ CardID cardID, /* s1 17 */ char *dirName, /* s2 18 */ char *fileName, /* s3 19 */ bool notifyUser) {}
/* 00220be0 00000120 */ void Realmc::TaskCheckDirectory::Start(/* s2 18 */ TaskCheckDirectory *this, /* s0 16 */ CardID cardID, /* s1 17 */ char *dirName) {}
/* 00220d00 0000012c */ void Realmc::TaskChangeDirectory::Start(/* s3 19 */ TaskChangeDirectory *this, /* s0 16 */ CardID cardID, /* s2 18 */ char *dirName, /* s1 17 */ bool notifyUser) {}
/* 00220e30 00000028 */ void Realmc::MessageTimer::Stop(/* v0 2 */ MessageTimer *this) {}
/* 00220e58 00000054 */ void Realmc::PS2Message::Clear(/* s0 16 */ PS2Message *this) {}
/* 00220eb0 00000044 */ void Realmc::Ps2Task::Init(/* v1 3 */ Ps2Task *this, /* v0 2 */ bool notifyUser) {}
/* 00220ef8 00000090 */ void Realmc::TaskCheckSpace::Clear(/* s0 16 */ TaskCheckSpace *this) {}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\cmn\interfaceimp.cpp
// *****************************************************************************

/* 00220f88 0000010c */ static bool IsValidVersion(/* 0x0(sp) */ short int *pMsg) {
	/* 0x4(sp) */ bool validVersion;
	/* 0x8(sp) */ bool foundVersion;
}

/* 00221098 000001a0 */ static int GetVersionNumber(/* 0x0(sp) */ short int *&pMsg) {
	/* 0x4(sp) */ int ver;
	/* 0x8(sp) */ bool found;
}

/* 00221238 000003a0 */ void Realmc::InterfaceImp::CheckVersion(/* s0 16 */ InterfaceImp *this) {
	/* 0x0(sp) */ short int *pMsgVer;
	/* 0x4(sp) */ short int *pMinRealMCVer;
	/* 0x8(sp) */ int version;
	/* 0xc(sp) */ int major;
	/* 0x10(sp) */ int minor;
	/* 0x14(sp) */ int patch;
	/* 0x20(sp) */ char msg[512];
}

/* 002215d8 000002fc */ InterfaceImp* Realmc::InterfaceImp::InterfaceImp(/* s1 17 */ InterfaceImp *this, /* 0x0(sp) */ ConfigSettings &settings) {}
/* 002218d8 00000198 */ void Realmc::InterfaceImp::~InterfaceImp(/* s0 16 */ InterfaceImp *this, /* 0x0(sp) */ int __in_chrg) {}
/* 00221a70 00000040 */ int Realmc::InterfaceImp::AddRef(/* v1 3 */ InterfaceImp *this) {}
/* 00221ab0 000000d8 */ int Realmc::InterfaceImp::Release(/* s1 17 */ InterfaceImp *this) {}
/* 00221b88 00000058 */ Interface* Realmc::Interface::Interface(/* s0 16 */ Interface *this) {}
/* 00221be0 0000005c */ void Realmc::Interface::~Interface(/* s1 17 */ Interface *this, /* s0 16 */ int __in_chrg) {}
/* 00221c40 00000064 */ void* Realmc::InterfaceImp::operator new(/* s0 16 */ size_t size) {}
/* 00221ca8 00000050 */ void Realmc::InterfaceImp::operator delete(/* s0 16 */ void *ptr, /* s1 17 */ size_t size) {}
/* 00221cf8 00000064 */ void* Realmc::InterfaceImp::operator new [](/* s0 16 */ size_t size) {}
/* 00221d60 00000050 */ void Realmc::InterfaceImp::operator delete [](/* s0 16 */ void *ptr, /* s1 17 */ size_t size) {}
/* 00221db0 00000034 */ void* Realmc::InterfaceImp::operator new(/* a0 4 */ void *ptr) {}
/* 00221de8 00000028 */ void Realmc::InterfaceImp::operator delete() {}
/* 00221e10 00000034 */ void* Realmc::InterfaceImp::operator new [](/* a0 4 */ void *ptr) {}
/* 00221e48 00000028 */ void Realmc::InterfaceImp::operator delete []() {}
/* 00221e70 00000064 */ void Realmc::InterfaceImp::LockInterfaceMutex(/* s0 16 */ InterfaceImp *this) {}
/* 00221ed8 00000064 */ void Realmc::InterfaceImp::UnlockInterfaceMutex(/* s0 16 */ InterfaceImp *this) {}
/* 00221f40 0000003c */ BaseInterface* Realmc::BaseInterface::BaseInterface(/* v1 3 */ BaseInterface *this) {}
/* 00221f80 0000006c */ void Realmc::BaseInterface::~BaseInterface(/* s1 17 */ BaseInterface *this, /* s0 16 */ int __in_chrg) {}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\cmn\locale.cpp
// *****************************************************************************

/* 00221ff0 00000030 */ void Realmc::Locale::SetLocaleGetStrCallback(/* 0x0(sp) */ LocaleCallback cb) {}

/* 00222020 0000086c */ __wchar_t* Realmc::Locale::GetString(/* 0x0(sp) */ int strID, /* 0x4(sp) */ char *parameterTypes) {
	/* 0x8(sp) */ int numParameters;
	/* 0xc(sp) */ int MAX_LOCALE_PARAMETERS;
	/* 0x10(sp) */ va_list val;
	/* 0x20(sp) */ int param[8];
	/* 0x40(sp) */ char *pType;
	/* 0x48(sp) */ char *pStr;
	/* 0x150(sp) */ int bufindex;
	/* 0x154(sp) */ short int *stri;
	/* 0x158(sp) */ short int *strd;
	/* 0x50(sp) */ char numstr[20];
	/* 0x15c(sp) */ bool nextcmd;
	/* 0x160(sp) */ int pi;
	/* 0x44(sp) */ int i;
	/* 0x50(sp) */ char msg[256];
	/* 0x164(sp) */ short int *s;
	/* 0x164(sp) */ int argval;
	/* 0x168(sp) */ int ibackup;
	/* 0x16c(sp) */ int numdigits;
	/* 0x170(sp) */ char *cptr;
	/* 0x70(sp) */ char msg[128];
}

/* 00222890 0000033c */ void Realmc::Locale::ConvertAsciiToUnicode(/* 0x0(sp) */ char *src, /* 0x4(sp) */ short unsigned int *dst) {
	/* 0x8(sp) */ bool isunicodemarkup;
	/* 0xc(sp) */ int unicode;
	/* 0x10(sp) */ int digit;
	/* 0x14(sp) */ int i;
}

/* 00222bd0 00000078 */ int Realmc::Locale::GetWstrLength(/* 0x0(sp) */ __wchar_t *str) {
	/* 0x4(sp) */ int len;
}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\cmn\memcard_memvectors.cpp
// *****************************************************************************

/* 00222c48 00000030 */ void Realmc::SetMemAllocator(/* 0x0(sp) */ IAllocator *allocator) {}
/* 00222c78 0000014c */ void* Realmc::DefaultSizeAlloc(/* 0x0(sp) */ char *pBlockName, /* 0x4(sp) */ int size, /* 0x8(sp) */ int align) {}
/* 00222dc8 000000d8 */ void Realmc::DefaultSizeFree(/* 0x0(sp) */ void *pBlock, /* 0x4(sp) */ int size) {}
/* 00222ea0 0000014c */ void* Realmc::AllocateMem(/* 0x0(sp) */ char *pBlockName, /* 0x4(sp) */ int size, /* 0x8(sp) */ int align) {}
/* 00222ff0 0000014c */ void* Realmc::AllocateMemSize(/* 0x0(sp) */ char *pBlockName, /* 0x4(sp) */ int size, /* 0x8(sp) */ int align) {}
/* 00223140 000000d4 */ void Realmc::FreeMem(/* 0x0(sp) */ void *pBlock) {}
/* 00223218 000000d8 */ void Realmc::FreeMemSize(/* 0x0(sp) */ void *pBlock, /* 0x4(sp) */ int size) {}
/* 002232f0 00000044 */ TagValuePair* EA::TagValuePair::TagValuePair(/* a3 7 */ TagValuePair *this, /* v1 3 */ unsigned int tag, /* a0 4 */ int value) {}
/* 00223338 00000044 */ TagValuePair* EA::TagValuePair::TagValuePair(/* a3 7 */ TagValuePair *this, /* v1 3 */ unsigned int tag, /* a0 4 */ void *value) {}
/* 00223380 00000038 */ TagValuePair& EA::TagValuePair::operator+(/* v1 3 */ TagValuePair *this, /* a0 4 */ TagValuePair &rhs) {}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\cmn\task.cpp
// *****************************************************************************

/* 002233b8 000000c0 */ void Realmc::AssertUnhandledTaskState(/* 0x0(sp) */ int taskState, /* 0x4(sp) */ char *pFile, /* 0x8(sp) */ int line) {
	/* 0x10(sp) */ char msg[512];
}

// *****************************************************************************
// FILE -- C:\packages\realmemcard\2.02.03\source\lib\ps2\ps2_blockcalculator.cpp
// *****************************************************************************

/* 00223478 00000088 */ BlockCalculatorImp* Realmc::BlockCalculatorImp::BlockCalculatorImp(/* s0 16 */ BlockCalculatorImp *this) {}
/* 00223500 00000030 */ void Realmc::BlockCalculatorImp::Clear(/* v0 2 */ BlockCalculatorImp *this) {}
/* 00223530 000004b4 */ void Realmc::BlockCalculatorImp::SetDirectoryInfo(/* s0 16 */ BlockCalculatorImp *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ DirectoryInfo &directoryInfo) {}
/* 002239e8 00000178 */ void Realmc::BlockCalculatorImp::AddDataFile(/* s0 16 */ BlockCalculatorImp *this, /* 0x0(sp) */ char *filename, /* 0x4(sp) */ unsigned int size) {}
/* 00223b60 00000114 */ unsigned int Realmc::BlockCalculatorImp::CalculateBlockSize(/* s1 17 */ BlockCalculatorImp *this, /* 0x0(sp) */ CardID &cardID, /* 0x4(sp) */ FileInfo &fileInfo) {}
/* 00223c78 00000044 */ unsigned int Realmc::BlockCalculatorImp::GetResult(/* v1 3 */ BlockCalculatorImp *this) {}
/* 00223cc0 00000038 */ CardID* Realmc::CardID::CardID(/* v1 3 */ CardID *this) {}
/* 00223cf8 00000060 */ DirectoryInfo* Realmc::DirectoryInfo::DirectoryInfo(/* v1 3 */ DirectoryInfo *this) {}
/* 00223d58 0000003c */ BlockCalculator* Realmc::BlockCalculator::BlockCalculator(/* v1 3 */ BlockCalculator *this) {}
/* 00223d98 0000005c */ void Realmc::BlockCalculatorImp::~BlockCalculatorImp(/* s1 17 */ BlockCalculatorImp *this, /* s0 16 */ int __in_chrg) {}
/* 00223df8 0000002c */ void Realmc::BlockCalculatorImp::SetBlockSize(/* v0 2 */ BlockCalculatorImp *this, /* v1 3 */ unsigned int blockSize) {}
/* 00223e28 00000060 */ unsigned int Realmc::BlockCalculatorImp::CalculateBlockSize(/* s0 16 */ BlockCalculatorImp *this, /* s1 17 */ unsigned int byteSize) {}
/* 00223e88 000000d0 */ unsigned int Realmc::BlockCalculatorImp::GetBlockCount(/* s1 17 */ BlockCalculatorImp *this, /* s0 16 */ unsigned int bytes) {}
/* 00223f58 0000006c */ void Realmc::BlockCalculator::~BlockCalculator(/* s1 17 */ BlockCalculator *this, /* s0 16 */ int __in_chrg) {}

// *****************************************************************************
// FILE -- C:\mywork\packages\sharedlogin\00.02.09-realmemcard-2.01.00\source\cmn\misc.cpp
// *****************************************************************************

/* 00223fc8 00000040 */ void EACSL::SFIOSetCardID(/* 0x0(sp) */ CardID &cardID, /* 0x10(sp) */ SFIODeviceInformationT deviceInfo) {}

/* 00224008 000000e0 */ void EACSL::SFIOInterfaceAddProcess(/* 0x0(sp) */ SFIOTestProcessTypeE eProcessType, /* 0x4(sp) */ LIBFunctionTypeE eFuncType) {
	/* 0x8(sp) */ SFIOTestProcessItem *item;
}

/* 002240e8 00000034 */ SFIOTestProcessItem* EACSL::SFIOTestProcessTypeQueue::Top(/* v1 3 */ SFIOTestProcessTypeQueue *this) {}
/* 00224120 0000006c */ void EACSL::SFIOTestProcessTypeQueue::Enqueue(/* v0 2 */ SFIOTestProcessTypeQueue *this, /* 0x0(sp) */ SFIOTestProcessItem *item) {}

/* 00224190 000000d8 */ SFIOTestProcessItem* EACSL::SFIOTestProcessTypeQueue::Dequeue(/* s0 16 */ SFIOTestProcessTypeQueue *this) {
	/* 0x0(sp) */ SFIOTestProcessItem *temp;
	/* 0x0(sp) */ SFIOTestProcessItem *temp;
}

/* 00224268 00000080 */ void EACSL::SFIOInterfaceRemoveProcess() {
	/* 0x0(sp) */ SFIOTestProcessItem *item;
}

/* 002242e8 0000004c */ Message& EACSL::GetCardMessage() {}
/* 00224338 00000050 */ _FileDiscp* EACSL::_FileDiscp::_FileDiscp(/* s0 16 */ _FileDiscp *this) {}
/* 00224388 000000e0 */ static void __static_initialization_and_destruction_0(/* 0x0(sp) */ int __initialize_p, /* 0x4(sp) */ int __priority) {}
/* 00224468 00000044 */ TagValuePair* EA::TagValuePair::TagValuePair(/* a3 7 */ TagValuePair *this, /* v1 3 */ unsigned int tag, /* a0 4 */ int value) {}
/* 002244b0 00000038 */ CardID* Realmc::CardID::CardID(/* v1 3 */ CardID *this) {}
/* 002244e8 0000003c */ SFIOTestProcessTypeQueue* EACSL::SFIOTestProcessTypeQueue::SFIOTestProcessTypeQueue(/* v1 3 */ SFIOTestProcessTypeQueue *this) {}
/* 00224528 00000060 */ void EACSL::SFIOTestProcessTypeQueue::~SFIOTestProcessTypeQueue(/* s0 16 */ SFIOTestProcessTypeQueue *this, /* s1 17 */ int __in_chrg) {}
/* 00224588 00000034 */ Message& EACSL::SFIOInterface::GetMessage(/* v1 3 */ SFIOInterface *this) {}
/* 002245c0 00000038 */ void _GLOBAL_$I$c__mywork_packages_sharedlogin_00.02.09_realmemcard_2.01.00_source_cmn_misc.cppWabaaa() {}
/* 002245f8 00000038 */ void _GLOBAL_$D$c__mywork_packages_sharedlogin_00.02.09_realmemcard_2.01.00_source_cmn_misc.cppIbcaaa() {}

// *****************************************************************************
// FILE -- C:\mywork\packages\sharedlogin\00.02.09-realmemcard-2.01.00\source\cmn\sfio_callbacks.cpp
// *****************************************************************************

/* 00224630 00000034 */ SFIOTerritoryE EACSL::SFIOCB::GetTerritoryCallback() {}

/* 00224668 000001c8 */ void EACSL::SFIOCB::Find(/* 0x0(sp) */ Char *pSaveName, /* 0x4(sp) */ SFIODeviceE eDevice) {
	/* 0x10(sp) */ SFIODeviceInformationT deviceInfo;
	/* 0x20(sp) */ SFIOErrorE eError;
	/* 0x30(sp) */ CardID cardID;
}

/* 00224830 000002bc */ void EACSL::SFIOCB::FreeSpace(/* 0x0(sp) */ SFIODeviceE eDevice) {
	/* 0x10(sp) */ SFIODeviceInformationT deviceInfo;
	/* 0x20(sp) */ SFIOErrorE eError;
	/* 0x30(sp) */ CardID cardID;
	/* 0x50(sp) */ bool done;
	/* 0x54(sp) */ int frameTime;
	/* 0x58(sp) */ Message *pMsg;
}

/* 00224af0 0000004c */ void EACSL::SFIOCB::FreeEntry(/* 0x0(sp) */ SFIODeviceE eDevice) {}

/* 00224b40 000001c0 */ void EACSL::SFIOCB::Mount(/* 0x0(sp) */ SFIODeviceE eDevice) {
	/* 0x4(sp) */ SFIOInterface *sfioinstance;
	/* 0x10(sp) */ SFIODeviceInformationT deviceInfo;
	/* 0x20(sp) */ SFIOErrorE eError;
	/* 0x30(sp) */ CardID cardID;
}

/* 00224d00 000000ac */ void EACSL::SFIOCB::UnMount(/* 0x0(sp) */ SFIODeviceE eDevice) {}

/* 00224db0 000003bc */ void EACSL::SFIOCB::Open(/* 0x0(sp) */ Char *pPath, /* 0x4(sp) */ Char *pFilename, /* 0x8(sp) */ SFIODeviceE eDevice, /* 0xc(sp) */ Uint32 uMode) {
	/* 0x10(sp) */ SFIODeviceInformationT deviceInfo;
	/* 0x20(sp) */ SFIOErrorE eError;
	/* 0x24(sp) */ FileInfo *fileInfo;
	/* 0x28(sp) */ DirectoryInfo *dirInfo;
	/* 0x2c(sp) */ Interface *realmc;
	/* 0x30(sp) */ CardID cardID;
	/* 0x50(sp) */ int i;
}

/* 00225170 000000b8 */ FileHandle EACSL::getInternalHandle(/* 0x0(sp) */ Int32 iHandle) {}

/* 00225228 0000014c */ void EACSL::SFIOCB::Close(/* 0x0(sp) */ Int32 iHandle) {
	/* 0x4(sp) */ FileHandle internalHandle;
	/* 0x8(sp) */ int i;
}

/* 00225378 000001cc */ void EACSL::SFIOCB::Create(/* 0x0(sp) */ Char *pSavename, /* 0x4(sp) */ Uint32 uSize, /* 0x8(sp) */ SFIODeviceE eDevice) {
	/* 0x10(sp) */ SFIODeviceInformationT deviceInfo;
	/* 0x20(sp) */ SFIOErrorE eError;
	/* 0x30(sp) */ CardID cardID;
}

/* 00225548 00000208 */ void EACSL::SFIOCB::Delete(/* 0x0(sp) */ Char *pPath, /* 0x4(sp) */ Char *pFilename, /* 0x8(sp) */ SFIODeviceE eDevice) {
	/* 0x10(sp) */ SFIODeviceInformationT deviceInfo;
	/* 0x20(sp) */ SFIOErrorE eError;
	/* 0x30(sp) */ CardID cardID;
}

/* 00225750 00000188 */ void EACSL::SFIOCB::Destroy(/* 0x0(sp) */ Char *pSavename, /* 0x4(sp) */ SFIODeviceE eDevice) {
	/* 0x10(sp) */ SFIODeviceInformationT deviceInfo;
	/* 0x20(sp) */ SFIOErrorE eError;
	/* 0x30(sp) */ CardID cardID;
}

/* 002258d8 000001c8 */ void EACSL::SFIOCB::Read(/* 0x0(sp) */ Int32 iHandle, /* 0x4(sp) */ void *pBuffer, /* 0x8(sp) */ Uint32 uNumBytes) {
	/* 0xc(sp) */ FileHandle internalHandle;
}

/* 00225aa0 00000148 */ void EACSL::SFIOCB::Write(/* 0x0(sp) */ Int32 iHandle, /* 0x4(sp) */ void *pBuffer, /* 0x8(sp) */ Uint32 uNumBytes) {
	/* 0xc(sp) */ FileHandle internalHandle;
}

/* 00225be8 00000164 */ void EACSL::SFIOCB::Seek(/* 0x0(sp) */ Int32 iHandle, /* 0x4(sp) */ Int32 iOffset, /* 0x8(sp) */ SFIOSeekE eSeek) {
	/* 0xc(sp) */ FileHandle internalHandle;
}

/* 00225d50 000000cc */ void EACSL::SFIOCB::Flush(/* 0x0(sp) */ Int32 iHandle) {
	/* 0x4(sp) */ FileHandle internalHandle;
}

/* 00225e20 00000050 */ void EACSL::SFIOCB::SetAttr(/* 0x0(sp) */ Int32 iHandle, /* 0x4(sp) */ Uint32 uAttr) {}
/* 00225e70 00000060 */ void EACSL::SFIOCB::GetFile(/* 0x0(sp) */ Char *pFilename) {}

/* 00225ed0 0000018c */ SFIOErrorE EACSL::SFIOCB::ProcessLocMessage(/* 0x0(sp) */ SFIOProcessE *pProcess, /* 0x4(sp) */ Int32 *pResult) {
	/* 0x8(sp) */ SFIOTestProcessItem *curr;
}

/* 00226060 00001ba8 */ SFIOErrorE EACSL::SFIOCB::Update(/* 0x0(sp) */ SFIOProcessE *pProcess, /* 0x4(sp) */ Int32 *pResult) {
	/* 0x8(sp) */ bool loopStatus;
	/* 0xc(sp) */ Message *msg;
	/* 0x10(sp) */ SFIOTestProcessItem *topItem;
	/* 0x14(sp) */ SFIOErrorE sfioerrorE;
	/* 0x18(sp) */ Message &cardmessage;
	/* 0x18(sp) */ SFIOErrorE sfioerrorE;
	/* 0x14(sp) */ Message &cardmessage;
	/* 0x18(sp) */ SFIOErrorE sfioerrorE;
	/* 0x14(sp) */ Message &cardmessage;
	/* 0x18(sp) */ SFIOErrorE sfioerrorE;
	/* 0x14(sp) */ Message &cardmessage;
	/* 0x1c(sp) */ int type;
	/* 0x20(sp) */ int cmd;
	/* 0x24(sp) */ int result;
	/* 0x24(sp) */ SFIOErrorE sfioerrorE;
	/* 0x20(sp) */ Message &cardmessage;
	/* 0x1c(sp) */ int type;
	/* 0x18(sp) */ int cmd;
	/* 0x14(sp) */ int result;
	/* 0x24(sp) */ SFIOErrorE sfioerrorE;
	/* 0x20(sp) */ Message &cardmessage;
	/* 0x24(sp) */ SFIOErrorE sfioerrorE;
	/* 0x20(sp) */ Message &cardmessage;
	/* 0x24(sp) */ SFIOErrorE sfioerrorE;
	/* 0x20(sp) */ Message &cardmessage;
	/* 0x24(sp) */ Message &cardmessage;
	/* 0x20(sp) */ SFIOErrorE sfioerrorE;
}

/* 00227c08 00000038 */ CardID* Realmc::CardID::CardID(/* v1 3 */ CardID *this) {}
/* 00227c40 00000034 */ FileInfo* EACSL::SFIOInterface::GetFileInfo(/* v1 3 */ SFIOInterface *this) {}
/* 00227c78 00000034 */ DirectoryInfo* EACSL::SFIOInterface::GetDirectoryInfo(/* v1 3 */ SFIOInterface *this) {}
/* 00227cb0 00000030 */ void EACSL::SFIOInterface::IncrementCheckingCounter(/* v0 2 */ SFIOInterface *this) {}
/* 00227ce0 00000034 */ int EACSL::SFIOInterface::GetCheckingCounter(/* v1 3 */ SFIOInterface *this) {}
/* 00227d18 00000034 */ bool EACSL::SFIOInterface::IsLoadingMessageEnabled(/* v1 3 */ SFIOInterface *this) {}
/* 00227d50 00000034 */ bool EACSL::SFIOInterface::IsSavingOnLoading(/* v1 3 */ SFIOInterface *this) {}
/* 00227d88 00000034 */ bool EACSL::SFIOInterface::IsCardChangedCheckEnabled(/* v1 3 */ SFIOInterface *this) {}
/* 00227dc0 00000034 */ LibMessage EACSL::GetMessageEnumeration(/* v1 3 */ Message *pMessage) {}
/* 00227df8 00000034 */ CardStatus EACSL::GetCardStatus(/* v1 3 */ Message *pMessage) {}
/* 00227e30 0000002c */ void EACSL::SetCardStatus(/* v0 2 */ Message *pMessage, /* v1 3 */ CardStatus status) {}

// *****************************************************************************
// FILE -- C:\mywork\packages\sharedlogin\00.02.09-realmemcard-2.01.00\source\cmn\sli.cpp
// *****************************************************************************

/* 00227e60 00000070 */ IAllocator* EACSL::GetAllocator() {}
/* 00227ed0 00000070 */ Interface* EACSL::GetRealmc() {}
/* 00227f40 0000008c */ SFIOInterface* EACSL::GetCurrentSFIOInstance() {}
/* 00227fd0 00000030 */ ConfigSettings* EACSL::GetConfigSettings() {}
/* 00228000 00000220 */ SharedLoginInterface* EACSL::SharedLoginInterface::CreateInstance(/* 0x0(sp) */ ConfigSettings &settings) {}
/* 00228220 00000088 */ static void __static_initialization_and_destruction_0(/* 0x0(sp) */ int __initialize_p, /* 0x4(sp) */ int __priority) {}
/* 002282a8 00000048 */ ConfigSettings* EACSL::ConfigSettings::ConfigSettings(/* v1 3 */ ConfigSettings *this) {}
/* 002282f0 000000dc */ ConfigSettings& EACSL::ConfigSettings::operator=(/* s2 18 */ ConfigSettings *this, /* s1 17 */ ConfigSettings &cs) {}
/* 002283d0 000000c0 */ void EACSL::ConfigSettings::~ConfigSettings(/* s2 18 */ ConfigSettings *this, /* s0 16 */ int __in_chrg) {}
/* 00228490 00000048 */ void _GLOBAL_$I$c__mywork_packages_sharedlogin_00.02.09_realmemcard_2.01.00_source_cmn_sli.cppCKaaaa() {}
/* 002284d8 00000048 */ void _GLOBAL_$D$c__mywork_packages_sharedlogin_00.02.09_realmemcard_2.01.00_source_cmn_sli.cpp4kbaaa() {}

// *****************************************************************************
// FILE -- C:\mywork\packages\sharedlogin\00.02.09-realmemcard-2.01.00\source\cmn\sli_imp.cpp
// *****************************************************************************

/* 00228520 00000148 */ SharedLoginInterfaceImp* EACSL::SharedLoginInterfaceImp::SharedLoginInterfaceImp(/* s0 16 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ ConfigSettings &settings) {}
/* 00228668 0000009c */ void EACSL::SharedLoginInterfaceImp::~SharedLoginInterfaceImp(/* s0 16 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ int __in_chrg) {}
/* 00228708 00000040 */ int EACSL::SharedLoginInterfaceImp::AddRef(/* v1 3 */ SharedLoginInterfaceImp *this) {}
/* 00228748 00000108 */ int EACSL::SharedLoginInterfaceImp::Release(/* s1 17 */ SharedLoginInterfaceImp *this) {}

/* 00228850 00000054 */ unsigned int EACSL::SharedLoginInterfaceImp::GetExpectedFileSize(/* s0 16 */ SharedLoginInterfaceImp *this) {
	/* 0x0(sp) */ unsigned int size;
}

/* 002288a8 00000138 */ void EACSL::SharedLoginInterfaceImp::Startup(/* s2 18 */ SharedLoginInterfaceImp *this) {}
/* 002289e0 00000100 */ void EACSL::SharedLoginInterfaceImp::Shutdown(/* s2 18 */ SharedLoginInterfaceImp *this) {}
/* 00228ae0 0000005c */ void EACSL::SharedLoginInterfaceImp::LoginInit(/* s0 16 */ SharedLoginInterfaceImp *this) {}
/* 00228b40 0000004c */ void EACSL::SharedLoginInterfaceImp::LoginShutdown(/* s0 16 */ SharedLoginInterfaceImp *this) {}
/* 00228b90 000000e0 */ void EACSL::SharedLoginInterfaceImp::CheckCard(/* s0 16 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ CardID &cardID) {}

/* 00228c70 000000a4 */ void EACSL::SharedLoginInterfaceImp::Load(/* s0 16 */ SharedLoginInterfaceImp *this) {
	/* 0x0(sp) */ SLoginErrorE errorE;
}

/* 00228d18 00000100 */ void EACSL::SharedLoginInterfaceImp::Save(/* s0 16 */ SharedLoginInterfaceImp *this) {
	/* 0x0(sp) */ SLoginErrorE errorE;
}

/* 00228e18 000000bc */ void EACSL::SharedLoginInterfaceImp::Create(/* s0 16 */ SharedLoginInterfaceImp *this) {
	/* 0x0(sp) */ SLoginErrorE errorE;
}

/* 00228ed8 00000068 */ void EACSL::SharedLoginInterfaceImp::SetNonQueryMessage(/* s0 16 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ EASBSpecialMessage specialmessageid) {}

/* 00228f40 000005d0 */ void EACSL::SharedLoginInterfaceImp::SetSharedLoginMessage(/* s4 20 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ unsigned int error, /* 0x4(sp) */ EASBSpecialMessage easbspecialmessage, /* 0x8(sp) */ Message *message, /* 0xc(sp) */ SHARED_LOGIN_STATE currentstate) {
	/* 0x10(sp) */ CARD_ERROR currentCardError;
	/* 0x14(sp) */ CARD_EXTENDED_ERROR currentCardExtendedError;
	/* 0x18(sp) */ int currentlibop;
}

/* 00229510 00000080 */ void EACSL::SharedLoginInterfaceImp::SetSpecialMessage(/* s0 16 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ EASBSpecialMessage specialmessageid) {}

/* 00229590 000001cc */ UpdateStatus EACSL::SharedLoginInterfaceImp::CheckSfioOperation(/* s0 16 */ SharedLoginInterfaceImp *this) {
	/* 0x0(sp) */ UpdateStatus status;
	/* 0x4(sp) */ SLoginProcessE sLoginProcessE;
	/* 0x8(sp) */ SLoginErrorE sErrorE;
}

/* 00229760 0000010c */ UpdateStatus EACSL::SharedLoginInterfaceImp::Update(/* s0 16 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ unsigned int frameElapsedTicks) {}

/* 00229870 00000128 */ void EACSL::SharedLoginInterfaceImp::CheckHardwareOp(/* s0 16 */ SharedLoginInterfaceImp *this) {
	/* 0x0(sp) */ int cmd;
	/* 0x4(sp) */ int result;
	/* 0x8(sp) */ int scemcsyncval;
}

/* 00229998 0000024c */ void EACSL::SharedLoginInterfaceImp::SetNextState(/* s1 17 */ SharedLoginInterfaceImp *this) {}
/* 00229be8 00000070 */ void EACSL::SharedLoginInterfaceImp::HandleError(/* s0 16 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ unsigned int error) {}

/* 00229c58 00000128 */ void EACSL::SharedLoginInterfaceImp::Format(/* s0 16 */ SharedLoginInterfaceImp *this) {
	/* 0x0(sp) */ int type;
	/* 0x4(sp) */ int cmd;
	/* 0x8(sp) */ int result;
}

/* 00229d80 000000c4 */ void EACSL::SharedLoginInterfaceImp::Delete(/* s0 16 */ SharedLoginInterfaceImp *this) {
	/* 0x0(sp) */ SLoginErrorE errorE;
}

/* 00229e48 000003a4 */ void EACSL::SharedLoginInterfaceImp::SendUserResponse(/* s3 19 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ RESPONSE_OPTION option) {}
/* 0022a1f0 00000058 */ void EACSL::SharedLoginInterfaceImp::DeleteForOverwrite(/* s0 16 */ SharedLoginInterfaceImp *this) {}
/* 0022a248 000000a4 */ void EACSL::SharedLoginInterfaceImp::SetSharedLoginData(/* s0 16 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ SharedLoginData &data) {}
/* 0022a2f0 00000098 */ void EACSL::SharedLoginInterfaceImp::GetSharedLoginData(/* s0 16 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ SharedLoginData *data) {}
/* 0022a388 0000008c */ void EACSL::SharedLoginInterfaceImp::GetDeleteVerification(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a418 00000048 */ void EACSL::SharedLoginInterfaceImp::GetDeleteMessage(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a460 0000008c */ void EACSL::SharedLoginInterfaceImp::GetDeleteDoubleVerification(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a4f0 0000006c */ void EACSL::SharedLoginInterfaceImp::GetDeleteComplete(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a560 0000005c */ void EACSL::SharedLoginInterfaceImp::GetSavingMessage(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a5c0 00000048 */ void EACSL::SharedLoginInterfaceImp::GetLoadingMessage(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a608 00000048 */ void EACSL::SharedLoginInterfaceImp::GetCheckingMessage(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a650 0000006c */ void EACSL::SharedLoginInterfaceImp::GetSavingComplete(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a6c0 0000006c */ void EACSL::SharedLoginInterfaceImp::GetLoadingFailed(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a730 00000048 */ void EACSL::SharedLoginInterfaceImp::GetLoadingWarning(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a778 0000006c */ void EACSL::SharedLoginInterfaceImp::GetDeleteFailedNoBio(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a7e8 0000006c */ void EACSL::SharedLoginInterfaceImp::GetSavingFailedUnknown(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a858 0000006c */ void EACSL::SharedLoginInterfaceImp::GetDeleteFailedUnknown(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a8c8 00000048 */ void EACSL::SharedLoginInterfaceImp::GetFormattingWarning(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a910 0000006c */ void EACSL::SharedLoginInterfaceImp::GetFormattingFailed(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a980 0000006c */ void EACSL::SharedLoginInterfaceImp::GetFileCorrupt(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022a9f0 000000a8 */ void EACSL::SharedLoginInterfaceImp::GetFileNeverLoaded(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022aa98 0000006c */ void EACSL::SharedLoginInterfaceImp::GetNotEnoughSpace(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022ab08 0000006c */ void EACSL::SharedLoginInterfaceImp::GetNoDeviceNoCard(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022ab78 0000006c */ void EACSL::SharedLoginInterfaceImp::GetNoDeviceNoCardDelete(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022abe8 0000008c */ void EACSL::SharedLoginInterfaceImp::GetNoDeviceCardNotFormattedSave(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022ac78 0000006c */ void EACSL::SharedLoginInterfaceImp::GetNoDeviceCardNotFormatted(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022ace8 0000006c */ void EACSL::SharedLoginInterfaceImp::GetCardChanged(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022ad58 0000008c */ void EACSL::SharedLoginInterfaceImp::GetFileOverwrite(/* v0 2 */ SharedLoginInterfaceImp *this, /* 0x0(sp) */ Message *message) {}
/* 0022ade8 00000098 */ type_info& EACSL::SharedLoginInterfaceImp type_info function() {}
/* 0022ae80 00000044 */ TagValuePair* EA::TagValuePair::TagValuePair(/* a3 7 */ TagValuePair *this, /* v1 3 */ unsigned int tag, /* a0 4 */ int value) {}
/* 0022aec8 0000004c */ SharedLoginData* EACSL::SharedLoginData::SharedLoginData(/* s0 16 */ SharedLoginData *this) {}
/* 0022af18 0000007c */ SharedLoginData& EACSL::SharedLoginData::operator=(/* s1 17 */ SharedLoginData *this, /* s0 16 */ SharedLoginData &rhs) {}
/* 0022af98 0000002c */ void EACSL::SharedLoginData::Reset(/* v0 2 */ SharedLoginData *this) {}

/* 0022afc8 000000ac */ void EACSL::Message::Reset(/* v0 2 */ Message *this) {
	/* 0x0(sp) */ unsigned int i;
}

/* 0022b078 0000003c */ SharedLoginInterface* EACSL::SharedLoginInterface::SharedLoginInterface(/* v1 3 */ SharedLoginInterface *this) {}
/* 0022b0b8 00000028 */ void EACSL::SFIOInterface::GetDamagedCard(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022b0e0 00000028 */ void EACSL::SFIOInterface::GetLoadedButDeleted(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022b108 00000028 */ void EACSL::SFIOInterface::GetNoFileButLoaded(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022b130 00000028 */ void EACSL::SFIOInterface::GetWrongFileDelete(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022b158 00000028 */ void EACSL::SFIOInterface::GetWrongFileSave(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022b180 00000028 */ void EACSL::SFIOInterface::GetFormatFailedNoDevice(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022b1a8 00000028 */ void EACSL::SFIOInterface::ClearCheckingCounter(/* v0 2 */ SFIOInterface *this) {}
/* 0022b1d0 00000034 */ int EACSL::SFIOInterface::GetCheckingCounter(/* v1 3 */ SFIOInterface *this) {}
/* 0022b208 00000028 */ void EACSL::SFIOInterface::DisableLoadingMessage(/* v0 2 */ SFIOInterface *this) {}
/* 0022b230 0000002c */ void EACSL::SFIOInterface::EnableLoadingMessage(/* v0 2 */ SFIOInterface *this) {}
/* 0022b260 00000028 */ void EACSL::SFIOInterface::DisableCardChangedCheck(/* v0 2 */ SFIOInterface *this) {}
/* 0022b288 0000002c */ void EACSL::SFIOInterface::EnableCardChangedCheck(/* v0 2 */ SFIOInterface *this) {}
/* 0022b2b8 00000034 */ SFIOCallbacksT* EACSL::SFIOInterface::GetSFIOcallbacks(/* v1 3 */ SFIOInterface *this) {}
/* 0022b2f0 00000034 */ UpdateStatus EACSL::Task::GetStatus(/* v1 3 */ Task *this) {}
/* 0022b328 000000d4 */ void* EACSL::SharedLoginInitData::operator new(/* s0 16 */ size_t s) {}
/* 0022b400 00000078 */ void EACSL::SharedLoginInitData::operator delete(/* s0 16 */ void *p) {}
/* 0022b478 000000d4 */ void* EACSL::SharedLoginInterfaceImp::operator new(/* s0 16 */ size_t s) {}
/* 0022b550 00000078 */ void EACSL::SharedLoginInterfaceImp::operator delete(/* s0 16 */ void *p) {}
/* 0022b5c8 00000054 */ void EACSL::SharedLoginInterfaceImp::GetMessage(/* s0 16 */ SharedLoginInterfaceImp *this, /* s1 17 */ Message *message) {}
/* 0022b620 00000070 */ type_info& EACSL::SharedLoginInterface type_info function() {}
/* 0022b690 00000070 */ type_info& EACSL::SFIOInterface type_info function() {}
/* 0022b700 00000044 */ TagValuePair* EA::TagValuePair::TagValuePair(/* a3 7 */ TagValuePair *this, /* v1 3 */ unsigned int tag, /* a0 4 */ void *value) {}
/* 0022b748 00000038 */ TagValuePair& EA::TagValuePair::operator+(/* v1 3 */ TagValuePair *this, /* a0 4 */ TagValuePair &rhs) {}
/* 0022b780 00000108 */ void EACSL::SFIOInterface::GetMessage(/* s0 16 */ SFIOInterface *this, /* s1 17 */ Message *msg) {}

// *****************************************************************************
// FILE -- C:\mywork\packages\sharedlogin\00.02.09-realmemcard-2.01.00\source\cmn\task_checkcard.cpp
// *****************************************************************************

/* 0022b888 000000f4 */ TaskCheckCard* EACSL::TaskCheckCard::TaskCheckCard(/* s1 17 */ TaskCheckCard *this, /* 0x0(sp) */ CardID &mCardID) {}
/* 0022b980 00000070 */ void EACSL::TaskCheckCard::~TaskCheckCard(/* s0 16 */ TaskCheckCard *this, /* 0x0(sp) */ int __in_chrg) {}

/* 0022b9f0 00000128 */ void EACSL::TaskCheckCard::Update(/* s0 16 */ TaskCheckCard *this, /* 0x0(sp) */ unsigned int frameElapsedTicks) {
	/* 0x4(sp) */ Message &msg;
}

/* 0022bb18 000002f8 */ void EACSL::TaskCheckCard::CheckCard(/* s3 19 */ TaskCheckCard *this, /* 0x0(sp) */ unsigned int frameElapsedTicks) {
	/* 0x4(sp) */ Message *pMsg;
	/* 0x10(sp) */ char buf[512];
	/* 0x210(sp) */ Message &msg;
	/* 0x210(sp) */ Message &msg;
}

/* 0022be10 00000080 */ type_info& EACSL::TaskCheckCard type_info function() {}

/* 0022be90 000000ac */ void EACSL::Message::Reset(/* v0 2 */ Message *this) {
	/* 0x0(sp) */ unsigned int i;
}

/* 0022bf40 00000048 */ Task* EACSL::Task::Task(/* a0 4 */ Task *this, /* v1 3 */ CardID &cardID) {}
/* 0022bf88 0000006c */ void EACSL::Task::~Task(/* s1 17 */ Task *this, /* s0 16 */ int __in_chrg) {}
/* 0022bff8 000000d4 */ void* EACSL::TaskCheckCard::operator new(/* s0 16 */ size_t s) {}
/* 0022c0d0 00000078 */ void EACSL::TaskCheckCard::operator delete(/* s0 16 */ void *p) {}
/* 0022c148 00000034 */ LibMessage EACSL::GetMessageEnumeration(/* v1 3 */ Message *pMessage) {}
/* 0022c180 00000034 */ unsigned int EACSL::GetMessageStringID(/* v1 3 */ Message *pMessage) {}
/* 0022c1b8 00000034 */ CardStatus EACSL::GetCardStatus(/* v1 3 */ Message *pMessage) {}
/* 0022c1f0 00000070 */ type_info& EACSL::Task type_info function() {}
/* 0022c260 00000044 */ TagValuePair* EA::TagValuePair::TagValuePair(/* a3 7 */ TagValuePair *this, /* v1 3 */ unsigned int tag, /* a0 4 */ int value) {}
/* 0022c2a8 00000044 */ TagValuePair* EA::TagValuePair::TagValuePair(/* a3 7 */ TagValuePair *this, /* v1 3 */ unsigned int tag, /* a0 4 */ void *value) {}
/* 0022c2f0 00000038 */ TagValuePair& EA::TagValuePair::operator+(/* v1 3 */ TagValuePair *this, /* a0 4 */ TagValuePair &rhs) {}

// *****************************************************************************
// FILE -- C:\mywork\packages\sharedlogin\00.02.09-realmemcard-2.01.00\source\cmn\sfio_interface.cpp
// *****************************************************************************

/* 0022c328 00000180 */ SFIOInterface* EACSL::SFIOInterface::SFIOInterface(/* s0 16 */ SFIOInterface *this, /* 0x0(sp) */ IAllocator *allocator, /* 0x4(sp) */ Interface *realmc) {}
/* 0022c4a8 00000068 */ void EACSL::SFIOInterface::~SFIOInterface(/* s0 16 */ SFIOInterface *this, /* 0x0(sp) */ int __in_chrg) {}
/* 0022c510 00000054 */ void EACSL::SFIOInterface::ClearSFIOParams(/* v0 2 */ SFIOInterface *this) {}
/* 0022c568 00000424 */ void EACSL::SFIOInterface::SetSpecialSFIOMessage(/* s7 23 */ SFIOInterface *this, /* 0x0(sp) */ EASBSpecialMessage &easbspecialmessage, /* 0x4(sp) */ Message *message) {}
/* 0022c990 00000070 */ type_info& EACSL::SFIOInterface type_info function() {}
/* 0022ca00 0000004c */ Message* EACSL::Message::Message(/* s0 16 */ Message *this) {}
/* 0022ca50 00000028 */ void EACSL::SFIOInterface::GetDeleteDoubleVerification(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022ca78 00000028 */ void EACSL::SFIOInterface::GetDeleteVerification(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022caa0 00000028 */ void EACSL::SFIOInterface::GetDeleteComplete(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cac8 00000028 */ void EACSL::SFIOInterface::GetSavingMessage(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022caf0 00000028 */ void EACSL::SFIOInterface::GetLoadingMessage(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cb18 00000028 */ void EACSL::SFIOInterface::GetCheckingMessage(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cb40 00000028 */ void EACSL::SFIOInterface::GetSavingComplete(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cb68 00000028 */ void EACSL::SFIOInterface::GetLoadingFailed(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cb90 00000028 */ void EACSL::SFIOInterface::GetLoadingWarning(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cbb8 00000028 */ void EACSL::SFIOInterface::GetDeleteFailedNoBio(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cbe0 00000028 */ void EACSL::SFIOInterface::GetSavingFailedUnknown(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cc08 00000028 */ void EACSL::SFIOInterface::GetDeleteFailedUnknown(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cc30 00000028 */ void EACSL::SFIOInterface::GetFormattingWarning(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cc58 00000028 */ void EACSL::SFIOInterface::GetFormattingFailed(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cc80 00000028 */ void EACSL::SFIOInterface::GetDamagedCard(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cca8 00000028 */ void EACSL::SFIOInterface::GetFileCorrupt(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022ccd0 00000028 */ void EACSL::SFIOInterface::GetFileNeverLoaded(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022ccf8 00000028 */ void EACSL::SFIOInterface::GetLoadedButDeleted(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cd20 00000028 */ void EACSL::SFIOInterface::GetNoFileButLoaded(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cd48 00000028 */ void EACSL::SFIOInterface::GetNotEnoughSpace(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cd70 00000028 */ void EACSL::SFIOInterface::GetNoDeviceNoCard(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cd98 00000028 */ void EACSL::SFIOInterface::GetNoDeviceNoCardDelete(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cdc0 00000028 */ void EACSL::SFIOInterface::GetNoDeviceCardNotFormattedSave(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cde8 00000028 */ void EACSL::SFIOInterface::GetNoDeviceCardNotFormatted(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022ce10 00000028 */ void EACSL::SFIOInterface::GetFileOverwrite(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022ce38 00000028 */ void EACSL::SFIOInterface::GetWrongFileDelete(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022ce60 00000028 */ void EACSL::SFIOInterface::GetWrongFileSave(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022ce88 00000028 */ void EACSL::SFIOInterface::GetDeleteMessage(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022ceb0 00000028 */ void EACSL::SFIOInterface::GetCardChanged(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022ced8 00000028 */ void EACSL::SFIOInterface::GetFormatFailedNoDevice(/* v0 2 */ SFIOInterface *this, /* v1 3 */ Message *cardmessage) {}
/* 0022cf00 00000108 */ void EACSL::SFIOInterface::GetMessage(/* s0 16 */ SFIOInterface *this, /* s1 17 */ Message *msg) {}

/* 0022d008 000000ac */ void EACSL::Message::Reset(/* v0 2 */ Message *this) {
	/* 0x0(sp) */ unsigned int i;
}

// *****************************************************************************
// FILE -- D:\packages\EASSDK\dev\source\common\SharedLogin\SharedLogin.c
// *****************************************************************************

/* 0022d0b8 00000014 */ static SLoginErrorE _SLoginStaticDataStartup(/* a0 4 */ SLoginInitParamsT *pInitParams) {}
/* 0022d0d0 00000010 */ static SLoginErrorE _SLoginStaticDataShutdown() {}
/* 0022d0e0 00000140 */ static SLoginErrorE _SLoginDynamicDataStartup(/* s1 17 */ SLoginInitParamsT *pInitParams) {}
/* 0022d220 0000003c */ static SLoginErrorE _SLoginDynamicDataShutdown() {}

/* 0022d260 000000b0 */ static SLoginErrorE _SLoginTagFileStartup() {
	/* 0x0(sp) */ Char strEncryptionKey[26];
	/* 0x20(sp) */ TagFileParamsT Params;
}

/* 0022d310 00000024 */ static SLoginErrorE _SLoginTagFileShutdown() {}

/* 0022d338 00000050 */ static SLoginErrorE _SLoginStateStart(/* a1 5 */ SLoginStateE eSLoginState) {
	/* s0 16 */ SLoginErrorE eSLoginError;
}

/* 0022d388 00000088 */ static SLoginErrorE _SLoginStateProcessCB(/* a0 4 */ SLoginProcessE *peSLoginProcess) {
	/* 0x0(sp) */ SLoginErrorE eSLoginError;
}

/* 0022d410 00000268 */ static SLoginErrorE _SLoginActionTagFileProcessCB(/* s0 16 */ SLoginProcessE *peSLoginProcess) {
	/* 0xc(sp) */ SLoginErrorE eSLoginError;
	/* 0x0(sp) */ SLoginProcessE eProcessAbort;
	/* 0x4(sp) */ SLoginProcessE eProcess;
	/* 0x8(sp) */ SLoginProcessE eProcessAbort;
}

/* 0022d678 00000040 */ static void _SLoginActionTagFileProcessFailCB(/* a0 4 */ SLoginProcessE *peSLoginProcess, /* a3 7 */ SLoginErrorE *peSLoginError) {}
/* 0022d6b8 0000001c */ static void _SLoginActionTagFileCloseDeleteProcessFailCB(/* a0 4 */ SLoginProcessE *peSLoginProcess, /* a1 5 */ SLoginErrorE *peSLoginError) {}
/* 0022d6d8 000000b4 */ static void _SLoginActionTagFileProcessCompleteCB(/* a2 6 */ SLoginProcessE *peSLoginProcess, /* a1 5 */ SLoginErrorE *peSLoginError) {}
/* 0022d790 0000002c */ static void _SLoginActionTagFileCloseDeleteProcessCompleteCB(/* s0 16 */ SLoginProcessE *peSLoginProcess, /* a1 5 */ SLoginErrorE *peSLoginError) {}
/* 0022d7c0 00000030 */ static void _SLoginActionTagFileCloseErrorOccurredProcessCompleteCB(/* a0 4 */ SLoginProcessE *peSLoginProcess, /* a1 5 */ SLoginErrorE *peSLoginError) {}
/* 0022d7f0 00000088 */ static void _SLoginActionControlProcessStartCB(/* a2 6 */ SLoginProcessE *peSLoginProcess, /* a1 5 */ SLoginErrorE *peSLoginError) {}
/* 0022d878 00000048 */ static SLoginErrorE _SLoginStepTagFileCreateStartCB() {}
/* 0022d8c0 00000048 */ static SLoginErrorE _SLoginStepTagFileOpenStartCB() {}
/* 0022d908 00000040 */ static SLoginErrorE _SLoginStepTagFileDeleteStartCB() {}

/* 0022d948 000000b8 */ static SLoginErrorE _SLoginStepTagFileWriteAccountNameStartCB() {
	/* v0 2 */ SLoginErrorE eSLoginError;
}

/* 0022da00 000000b8 */ static SLoginErrorE _SLoginStepTagFileWriteAccountPasswordStartCB() {
	/* v0 2 */ SLoginErrorE eSLoginError;
}

/* 0022dab8 00000088 */ static SLoginErrorE _SLoginStepTagFileReadAccountNameStartCB() {
	/* v0 2 */ SLoginErrorE eSLoginError;
}

/* 0022db40 00000088 */ static SLoginErrorE _SLoginStepTagFileReadAccountPasswordStartCB() {
	/* v0 2 */ SLoginErrorE eSLoginError;
}

/* 0022dbc8 00000034 */ static SLoginErrorE _SLoginStepTagFileCloseStartCB() {}

/* 0022dc00 000000f8 */ static SLoginErrorE _SLoginStepTagFileProcessCB(/* s1 17 */ SLoginProcessE *peSLoginProcess) {
	/* a0 4 */ SLoginErrorE eSLoginError;
	/* 0x0(sp) */ SFIOProcessE eSFIOProcess;
	/* 0x4(sp) */ Int32 iResult;
}

/* 0022dcf8 00000008 */ static SLoginErrorE _SLoginStepTagFileAbortCB() {}
/* 0022dd00 0000002c */ static SLoginErrorE _SLoginStepTagFileCreateProcessCompleteCB() {}
/* 0022dd30 0000001c */ static SLoginErrorE _SLoginStepTagFileOpenProcessCompleteCB() {}
/* 0022dd50 00000054 */ static SLoginErrorE _SLoginStepTagFileReadAccountNameProcessCompleteCB() {}
/* 0022dda8 00000054 */ static SLoginErrorE _SLoginStepTagFileReadAccountPasswordProcessCompleteCB() {}
/* 0022de00 0000001c */ static SLoginErrorE _SLoginStepTagFileWriteAccountNameProcessCompleteCB() {}
/* 0022de20 0000001c */ static SLoginErrorE _SLoginStepTagFileWriteAccountPasswordProcessCompleteCB() {}
/* 0022de40 00000038 */ static SLoginErrorE _SLoginStepTagFileReadWriteAccountNameProcessFailCB(/* a0 4 */ SLoginErrorE eSLoginError) {}
/* 0022de78 00000038 */ static SLoginErrorE _SLoginStepTagFileReadWriteAccountPasswordProcessFailCB(/* a0 4 */ SLoginErrorE eSLoginError) {}

/* 0022deb0 000000a4 */ static SLoginErrorE _SLoginStateCurrentStartup(/* a0 4 */ SLoginStateE eState) {
	/* a2 6 */ SLoginErrorE eSLoginError;
	/* a0 4 */ SLoginStepE eStepCBIndex;
}

/* 0022df58 00000030 */ static SLoginErrorE _SLoginStateCurrentShutdown() {}

/* 0022df88 00000184 */ static SLoginErrorE _SLoginActionCurrentStartup(/* a0 4 */ SLoginActionE eAction, /* a1 5 */ Bool bActionSave) {
	/* a3 7 */ SLoginErrorE eSLoginError;
}

/* 0022e110 00000034 */ static SLoginErrorE _SLoginActionCurrentShutdown() {}

/* 0022e148 000001a0 */ static SLoginErrorE _SLoginStepCurrentStartup() {
	/* a2 6 */ SLoginErrorE eSLoginError;
	/* a0 4 */ SLoginStepE eStepCBIndex;
}

/* 0022e2e8 00000050 */ static SLoginErrorE _SLoginStepCurrentShutdown() {
	/* a1 5 */ SLoginStepE eStepCBIndex;
}

/* 0022e338 000000b0 */ static SLoginErrorE _SLoginValidateIconData(/* s1 17 */ void *pIconBuffer) {
	/* s2 18 */ SLoginErrorE eSLoginError;
	/* a2 6 */ ChecksumErrorE eCkError;
	/* s0 16 */ ChecksumType_t *pChecksum;
	/* 0x0(sp) */ ChecksumResultT *pResult;
	/* v0 2 */ Uint32 uChecksum;
}

/* 0022e3e8 00000050 */ static Char* _SLoginStrnzcpy(/* a0 4 */ Char *strDst, /* a1 5 */ Char *strSrc, /* a2 6 */ Uint32 uSize) {
	/* v1 3 */ Char *strTemp;
}

/* 0022e438 00000034 */ SLoginErrorE SLoginInit(/* s0 16 */ SLoginInitParamsT *pInitParams) {}
/* 0022e470 0000002c */ SLoginErrorE SLoginShutdown() {}

/* 0022e4a0 000000c0 */ SLoginErrorE SLoginCreateStart(/* s3 19 */ void *pIcon, /* s1 17 */ SLoginAccountDataT *pAccountData) {
	/* s2 18 */ SLoginErrorE eSLoginError;
}

/* 0022e560 00000048 */ SLoginErrorE SLoginLoadStart(/* a0 4 */ SLoginAccountDataT *pAccountData) {
	/* a1 5 */ SLoginErrorE eSLoginError;
}

/* 0022e5a8 00000084 */ SLoginErrorE SLoginSaveStart(/* s1 17 */ SLoginAccountDataT *pAccountData) {
	/* s2 18 */ SLoginErrorE eSLoginError;
}

/* 0022e630 0000001c */ SLoginErrorE SLoginDeleteStart() {}

/* 0022e650 0000009c */ SLoginErrorE SLoginProcess(/* s0 16 */ SLoginProcessE *peSLoginProcess) {
	/* s2 18 */ SLoginErrorE eSLoginError;
}

/* 0022e6f0 00000084 */ SLoginErrorE SLoginConvertTagFileError(/* a0 4 */ TagErrorE eTagFileError) {
	/* v0 2 */ SLoginErrorE eSLoginError;
}

/* 0022e778 00000018 */ SLoginErrorE SLoginStatePreviousGet(/* a0 4 */ SLoginStateE *peSLoginState) {}
/* 0022e790 00000018 */ SLoginErrorE SLoginActionPreviousGet(/* a0 4 */ SLoginActionE *peSLoginAction) {}

/* 0022e7a8 00000050 */ SLoginErrorE SLoginFileSizeExpectedGet(/* s0 16 */ Uint32 *puFileSize) {
	/* 0x0(sp) */ SFIOSystemDataT SFIOSystemData;
	/* 0x3d0(sp) */ Uint32 uEntries;
}

// *****************************************************************************
// FILE -- D:\packages\EASSDK\dev\source\PS2\SharedLogin\LLSharedLogin.c
// *****************************************************************************

/* 0022e7f8 000002f8 */ SLoginErrorE LLSLoginSystemDataSetup(/* fp 30 */ void *pIcon) {
	/* 0x0(sp) */ SFIOSystemDataT SysData;
	/* v0 2 */ SLoginErrorE eSLoginError;
	/* 0x3d0(sp) */ _iconVu0IVECTOR _BgColor[4];
	/* 0x410(sp) */ _iconVu0FVECTOR _LightDir[3];
	/* 0x440(sp) */ _iconVu0FVECTOR _LightColor[3];
	/* 0x470(sp) */ _iconVu0FVECTOR _Ambient;
}

// *****************************************************************************
// FILE -- D:\packages\EASSDK\dev\source\common\SharedFileIO\SharedFileIO.c
// *****************************************************************************

/* 0022eaf0 00000038 */ SFIODeviceE SFIOGetFirstDevice(/* a0 4 */ Uint16 uDeviceMask) {
	/* v1 3 */ SFIODeviceE eDevice;
}

/* 0022eb28 00000034 */ SFIODeviceE SFIOGetLastDevice(/* a0 4 */ Uint16 uDeviceMask) {
	/* v1 3 */ SFIODeviceE eDevice;
}

/* 0022eb60 00000100 */ SFIODeviceE SFIOGetNextDevice(/* a0 4 */ Uint16 uDeviceMask, /* a1 5 */ SFIOSortE eSort) {
	/* a0 4 */ SFIODeviceE eDevice;
	/* a2 6 */ Int16 iDevice;
}

/* 0022ec60 00000034 */ Uint32 SFIOGetNumDevices(/* a3 7 */ Uint16 uDeviceMask) {
	/* a2 6 */ Uint32 uNumDevices;
	/* a1 5 */ SFIODeviceE eDevice;
}

/* 0022ec98 00000024 */ void SFIOSetLastError(/* a0 4 */ SFIOErrorE eError) {}
/* 0022ecc0 00000010 */ SFIOErrorE SFIOGetLastError() {}
/* 0022ecd0 0000004c */ SFIOErrorE SFIORecoverFlushStart(/* a0 4 */ SFIOFileDescriptorT *pDescriptor, /* a1 5 */ SFIOProcessE *pProcess) {}
/* 0022ed20 0000004c */ SFIOErrorE SFIORecoverCloseStart(/* a0 4 */ SFIOFileDescriptorT *pDescriptor, /* a1 5 */ SFIOProcessE *pProcess) {}
/* 0022ed70 0000004c */ SFIOErrorE SFIORecoverUnMountStart(/* a0 4 */ SFIODeviceE eDevice, /* a1 5 */ SFIOProcessE *pProcess) {}
/* 0022edc0 00000008 */ static SFIOErrorE _SFIOValidateNoAction(/* a0 4 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022edc8 00000040 */ static SFIOErrorE _SFIOValidateMount(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022ee08 00000050 */ static SFIOErrorE _SFIOValidateFind(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022ee58 00000090 */ static SFIOErrorE _SFIOValidateSeekReadWrite(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022eee8 00000068 */ static SFIOErrorE _SFIOValidateFlush(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022ef50 00000068 */ static SFIOErrorE _SFIOValidateClose(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022efb8 00000048 */ static SFIOErrorE _SFIOValidateUnMount(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022f000 00000058 */ static SFIOErrorE _SFIOValidateCreate(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022f058 00000070 */ static SFIOErrorE _SFIOValidateOpen(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022f0c8 00000068 */ static SFIOErrorE _SFIOValidateDelete(/* a0 4 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022f130 00000058 */ static SFIOErrorE _SFIOValidateDestroy(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022f188 00000048 */ static SFIOErrorE _SFIOValidateGetFreeSpace(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022f1d0 00000040 */ static SFIOErrorE _SFIOValidateGetFreeEntry(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022f210 00000050 */ static SFIOErrorE _SFIOValidateSetAttr(/* a1 5 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE eProcess, /* a2 6 */ Int32 iResult) {}
/* 0022f260 00000008 */ static SFIOErrorE _SFIOProcessNoAction(/* a0 4 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE *pProcess, /* a2 6 */ Int32 *pResult) {}

/* 0022f268 000002ec */ static SFIOErrorE _SFIOProcessOpen(/* s0 16 */ SFIOErrorE eError, /* s1 17 */ SFIOProcessE *pProcess, /* a2 6 */ Int32 *pResult) {
	/* 0x0(sp) */ Char aFullPath[65];
}

/* 0022f558 00000104 */ static SFIOErrorE _SFIOProcessSeekReadWrite(/* a3 7 */ SFIOErrorE eError, /* s0 16 */ SFIOProcessE *pProcess, /* a2 6 */ Int32 *pResult) {}
/* 0022f660 00000148 */ static SFIOErrorE _SFIOProcessClose(/* a3 7 */ SFIOErrorE eError, /* s0 16 */ SFIOProcessE *pProcess, /* a2 6 */ Int32 *pResult) {}
/* 0022f7a8 000000e0 */ static SFIOErrorE _SFIOProcessRecover(/* a0 4 */ SFIOErrorE eError, /* a1 5 */ SFIOProcessE *pProcess, /* a2 6 */ Int32 *pResult) {}
/* 0022f888 00000010 */ Bool SFIOIsInitialized() {}

/* 0022f898 00000350 */ SFIOErrorE SFIOInit(/* s3 19 */ SFIODeviceE *pDeviceList, /* s0 16 */ SFIOCallbacksT *pCallbacks, /* s2 18 */ Uint32 uMemID) {
	/* a0 4 */ SFIODeviceE *pDevice;
	/* s1 17 */ Uint8 uNumDevices;
	/* a0 4 */ Int8 iDevice;
}

/* 0022fbe8 000000d0 */ SFIOErrorE SFIOShutdown() {}
/* 0022fcb8 000000b4 */ SFIOErrorE SFIOGetStorageMediaInfo(/* a0 4 */ SFIOFileDescriptorT *pDescriptor) {}
/* 0022fd70 000001a8 */ SFIOErrorE SFIODeleteStart(/* s4 20 */ Char *pFilename, /* s0 16 */ SFIODeviceE eDevice, /* s2 18 */ SFIOSortE eSort) {}
/* 0022ff18 000001cc */ SFIOErrorE SFIOCreateStart(/* s3 19 */ Char *pFilename, /* s0 16 */ SFIODeviceE eDevice, /* s2 18 */ SFIOSortE eSort) {}
/* 002300e8 000001a8 */ SFIOErrorE SFIOOpenStart(/* s4 20 */ Char *pFilename, /* s0 16 */ SFIODeviceE eDevice, /* s2 18 */ SFIOSortE eSort) {}
/* 00230290 0000011c */ SFIOErrorE SFIOCloseStart(/* s0 16 */ SFIOFileDescriptorT *pDescriptor) {}
/* 002303b0 00000190 */ SFIOErrorE SFIOSeekStart(/* s0 16 */ SFIOFileDescriptorT *pDescriptor, /* s2 18 */ Int32 iOffset, /* s3 19 */ SFIOSeekE eSeek) {}
/* 00230540 00000158 */ SFIOErrorE SFIOReadStart(/* s0 16 */ SFIOFileDescriptorT *pDescriptor, /* s2 18 */ void *pBuffer, /* s1 17 */ Uint32 uNumBytes) {}
/* 00230698 00000158 */ SFIOErrorE SFIOWriteStart(/* s0 16 */ SFIOFileDescriptorT *pDescriptor, /* s2 18 */ void *pBuffer, /* s1 17 */ Uint32 uNumBytes) {}

/* 002307f0 000001ac */ SFIOErrorE SFIOProcess(/* s1 17 */ SFIOProcessE *pProcess, /* s2 18 */ Int32 *pResult) {
	/* s0 16 */ SFIOErrorE eError;
}

/* 002309a0 0000006c */ SFIOErrorE SFIOGetDeviceInformation(/* s0 16 */ SFIODeviceE eDevice, /* s1 17 */ SFIODeviceInformationT *pDeviceInfo) {}
/* 00230a10 00000124 */ SFIOErrorE SFIOSetSystemData(/* s0 16 */ SFIOSystemDataT *pData) {}
/* 00230b38 0000001c */ Bool SFIOValidateFilename(/* a0 4 */ Char *pFilename) {}

/* 00230b58 00000170 */ void SFIOStringToShiftJIS(/* a2 6 */ Char *pSrc, /* t1 9 */ Uint16 *pDest) {
	/* a1 5 */ Uint8 uDestPtr;
	/* a0 4 */ Uint16 uChar;
	/* 0x0(sp) */ Uint8 aTrans[33];
}

/* 00230cc8 000001b4 */ void SFIOShiftJISToString(/* t1 9 */ Uint16 *pSrc, /* t0 8 */ Char *pDest) {
	/* a0 4 */ Uint16 uChar;
	/* a1 5 */ Uint8 uDestPtr;
	/* a2 6 */ Int8 iLoop;
	/* v0 2 */ Uint16 uSwapByte;
	/* 0x0(sp) */ Uint8 aUnTrans[33];
}

/* 00230e80 0000001c */ void SFIOCalculateSaveSize(/* a0 4 */ SFIOSystemDataT *pData, /* a1 5 */ Uint32 *pSize, /* a2 6 */ Uint32 *pEntries) {}
/* 00230ea0 0000001c */ void SFIOGetSignatureMethod(/* a0 4 */ ChecksumType_t **pSignature) {}
/* 00230ec0 0000001c */ Uint32 SFIOGetReservedSize() {}

// *****************************************************************************
// FILE -- D:\packages\EASSDK\dev\source\PS2\SharedFileIO\llSharedFileIO.c
// *****************************************************************************

/* 00230ee0 00000094 */ void llSFIOGetDeviceInformation(/* a0 4 */ SFIODeviceE eDevice, /* a1 5 */ SFIODeviceInformationT *pDeviceInfo) {}

/* 00230f78 00000214 */ Char* llSFIOMakeSearchName(/* a0 4 */ Char *pSearchName, /* a1 5 */ SFIOSystemDataT *pSysData, /* s1 17 */ Char *pSharedName) {
	/* v1 3 */ SFIOTerritoryE territory;
	/* s0 16 */ Char *pCursor;
}

/* 00231190 000003e8 */ void llSFIOMakeNames(/* a0 4 */ Char *pDirName, /* s3 19 */ Char *pFileName, /* a2 6 */ SFIOSystemDataT *pSysData, /* s1 17 */ Char *pSharedName) {
	/* s2 18 */ SFIOTerritoryE territory;
	/* s0 16 */ Char *pCursor;
}

/* 00231578 000005d0 */ SFIOErrorE llSFIOProcessCreate(/* a3 7 */ SFIOErrorE eError, /* s0 16 */ SFIOProcessE *pProcess, /* a2 6 */ Int32 *pResult) {
	/* 0x0(sp) */ Char aFullPath[65];
}

/* 00231b48 00000478 */ SFIOErrorE llSFIOProcessDelete(/* s0 16 */ SFIOErrorE eError, /* s1 17 */ SFIOProcessE *pProcess, /* a2 6 */ Int32 *pResult) {
	/* 0x0(sp) */ Char aFullPath[65];
}

/* 00231fc0 00000008 */ Bool llSFIOValidateFilename(/* a0 4 */ Char *pFilename) {}

/* 00231fc8 00000048 */ void llSFIOCalculateSaveSize(/* a0 4 */ SFIOSystemDataT *pData, /* a1 5 */ Uint32 *pSize, /* a2 6 */ Uint32 *pEntries) {
	/* v1 3 */ Uint32 uDataSize;
	/* a0 4 */ Uint32 uIconSize;
	/* v0 2 */ Uint32 uFileSize;
}

/* 00232010 0000002c */ void llSFIOGetSignatureMethod(/* s0 16 */ ChecksumType_t **pSignature) {
	/* v0 2 */ ChecksumType_t *pInternalSignature;
}

/* 00232040 00000008 */ void llSFIOEnableSharing(/* a0 4 */ Bool bEnable) {}
/* 00232048 00000008 */ Uint32 llSFIOGetReservedSize() {}
/* 00232050 0000000c */ SFIOErrorE llSFIOValidateSystemData(/* a0 4 */ SFIOSystemDataT *pSysData) {}

// *****************************************************************************
// FILE -- D:\packages\EASSDK\dev\source\common\TagFile\TagFile.c
// *****************************************************************************

/* 00232060 00000014 */ static TagErrorE _TagFileConvertSFIOError(/* a0 4 */ SFIOErrorE eErr) {}
/* 00232078 00000014 */ static TagErrorE _TagFileConvertCryptError(/* a0 4 */ SFIOErrorE eErr) {}
/* 00232090 00000014 */ static TagErrorE _TagFileConvertChecksumError(/* a0 4 */ SFIOErrorE eErr) {}

/* 002320a8 00000040 */ static void _TagFileGetAlignementConstraints(/* a0 4 */ TagModeE eMode, /* a1 5 */ Uint32 *pAlignAddr, /* a2 6 */ Uint32 *pAlignSize) {
	/* a3 7 */ Uint32 uAlignAddr;
	/* v1 3 */ Uint32 uAlignSize;
}

/* 002320e8 00000078 */ static Uint32 _TagFileCalculatePaddedBufferSize(/* s0 16 */ Uint32 uNumBytes, /* a1 5 */ TagModeE eMode) {
	/* 0x0(sp) */ Uint32 uAlignAddr;
	/* 0x4(sp) */ Uint32 uAlignSize;
}

/* 00232160 00000044 */ static TagBufferT* _TagFileGetReservedArea(/* a0 4 */ void *pBuffer, /* a1 5 */ Bool bValidate) {
	/* s0 16 */ TagBufferT *pCursor;
}

/* 002321a8 00000044 */ static void _TagFileCreateTag(/* a0 4 */ TagBufferT *pTagBuffer, /* 0x0(sp) */ TagT Tag, /* 0x4(sp) */ Uint32 uNumBytes, /* a3 7 */ SFIOSignatureT *pChecksum) {}
/* 002321f0 00000030 */ static void _TagFileDestroyTag(/* a0 4 */ TagBufferT *pTagBuffer) {}
/* 00232220 00000050 */ static void _TagFileExtractTag(/* a0 4 */ TagBufferT *pTagBuffer, /* a1 5 */ TagT *pTag, /* a2 6 */ Uint32 *pNumBytes, /* a3 7 */ SFIOSignatureT *pChecksum) {}

/* 00232270 00000060 */ static _TagFileMapElementT* _TagFileLocateTag(/* a3 7 */ TagT Tag, /* a1 5 */ Uint32 uOccurence) {
	/* v1 3 */ _TagFileMapElementT *pCursor;
	/* v1 3 */ _TagFileMapElementT *pFirst;
	/* a0 4 */ _TagFileMapElementT *pLast;
	/* a2 6 */ Uint32 uFound;
}

/* 002322d0 00000060 */ static void _TagFileUpdateTag(/* t0 8 */ TagT Tag, /* a1 5 */ Uint32 uOccurence, /* a2 6 */ SFIOSignatureT *pChecksum) {
	/* v1 3 */ _TagFileMapElementT *pCursor;
	/* v1 3 */ _TagFileMapElementT *pFirst;
	/* a0 4 */ _TagFileMapElementT *pLast;
	/* a3 7 */ Uint32 uFound;
}

/* 00232330 00000084 */ static Int32 _TagFileAddTagToMap(/* a0 4 */ TagT Tag, /* t0 8 */ Uint32 uDataNumBytes, /* a2 6 */ SFIOSignatureT *pChecksum) {
	/* a3 7 */ _TagFileMapElementT *pCursor;
	/* v1 3 */ _TagFileMapElementT *pFirst;
	/* v0 2 */ _TagFileMapElementT *pLast;
	/* a1 5 */ Uint32 uPaddedSize;
}

/* 002323b8 00000094 */ static TagErrorE _TagFileCalculateChecksum(/* s2 18 */ void *pBuffer, /* s1 17 */ Uint32 uNumBytes, /* s3 19 */ ChecksumResultT **pResult) {
	/* s0 16 */ ChecksumType_t *pChecksum;
	/* v0 2 */ ChecksumErrorE eError;
}

/* 00232450 00000060 */ static void _TagFileCalculateFileID(/* s0 16 */ SFIOSignatureT *pFileID) {
	/* 0x0(sp) */ ChecksumResultT *pResult;
	/* a1 5 */ Uint32 uNumBytes;
}

/* 002324b0 000000a4 */ static TagErrorE _TagFileEncrypt(/* s2 18 */ void *pBuffer, /* s1 17 */ Uint32 uNumBytes) {
	/* v0 2 */ CryptErrorE eError;
}

/* 00232558 000000a4 */ static TagErrorE _TagFileDecrypt(/* s2 18 */ void *pBuffer, /* s1 17 */ Uint32 uNumBytes) {
	/* v0 2 */ CryptErrorE eError;
}

/* 00232600 00000048 */ static TagErrorE _TagFileOpenStart(/* a0 4 */ Char *pFilename, /* a1 5 */ SFIODeviceE eDevice, /* a2 6 */ SFIOSortE eSort, /* s0 16 */ _TagActionE eAction) {}
/* 00232648 0000003c */ static TagErrorE _TagFileParseInit() {}
/* 00232688 00000058 */ static TagErrorE _TagFileParseTag() {}

/* 002326e0 00000098 */ static TagErrorE _TagFileSkipData() {
	/* v0 2 */ TagErrorE eError;
	/* 0x0(sp) */ TagT DataTag;
	/* 0x4(sp) */ Uint32 uDataNumBytes;
}

/* 00232778 000000b8 */ static TagErrorE _TagFileWriteData() {
	/* v0 2 */ TagErrorE eError;
	/* s0 16 */ TagBufferT *pReservedBuffer;
}

/* 00232830 0000006c */ static TagErrorE _TagFileReadData() {
	/* s1 17 */ TagBufferT *pReservedBuffer;
}

/* 002328a0 00000100 */ static TagErrorE _TagFileValidateData() {
	/* v0 2 */ TagErrorE eError;
	/* 0x8(sp) */ ChecksumResultT *pResult;
	/* s0 16 */ TagBufferT *pReservedBuffer;
	/* 0x4(sp) */ Uint32 uDataNumBytes;
	/* 0x0(sp) */ TagT DataTag;
}

/* 002329a0 00000010 */ Bool TagFileIsInitialized() {}

/* 002329b0 00000190 */ TagErrorE TagFileInit(/* s1 17 */ TagFileParamsT *pParams) {
	/* v0 2 */ TagErrorE eTagError;
	/* v0 2 */ SFIOErrorE eSFIOError;
	/* v0 2 */ ChecksumErrorE eChecksumError;
	/* v0 2 */ CryptErrorE eCryptError;
}

/* 00232b40 00000148 */ TagErrorE TagFileShutdown() {
	/* v0 2 */ TagErrorE eTagError;
	/* v0 2 */ SFIOErrorE eSFIOError;
	/* v0 2 */ ChecksumErrorE eChecksumError;
	/* v0 2 */ CryptErrorE eCryptError;
}

/* 00232c88 00000034 */ TagErrorE TagFileCreateStart(/* a0 4 */ Char *pFilename, /* a1 5 */ SFIODeviceE eDevice, /* a2 6 */ SFIOSortE eSort) {}
/* 00232cc0 00000034 */ TagErrorE TagFileDeleteStart(/* a0 4 */ Char *pFilename, /* a1 5 */ SFIODeviceE eDevice, /* a2 6 */ SFIOSortE eSort) {}
/* 00232cf8 000000d8 */ TagErrorE TagFileReopenStart(/* t0 8 */ TagFileDescriptorT *pDescriptor) {}
/* 00232dd0 0000001c */ TagErrorE TagFileOpenStart(/* a0 4 */ Char *pFilename, /* a1 5 */ SFIODeviceE eDevice, /* a2 6 */ SFIOSortE eSort) {}
/* 00232df0 00000060 */ TagErrorE TagFileCloseStart(/* s0 16 */ TagFileDescriptorT *pDescriptor) {}

/* 00232e50 000001d0 */ TagErrorE TagFileWriteStart(/* s3 19 */ TagFileDescriptorT *pDescriptor, /* s0 16 */ TagT Tag, /* s5 21 */ Uint32 uOccurence, /* a3 7 */ void *pBuffer, /* t0 8 */ Uint32 uNumBytes) {
	/* 0x0(sp) */ ChecksumResultT *pChecksum;
	/* v0 2 */ TagErrorE eError;
	/* s2 18 */ _TagFileMapElementT *pTag;
	/* s0 16 */ Int32 iOffset;
}

/* 00233020 00000158 */ TagErrorE TagFileReadStart(/* s0 16 */ TagFileDescriptorT *pDescriptor, /* t3 11 */ TagT Tag, /* a1 5 */ Uint32 uOccurence, /* a3 7 */ void *pBuffer, /* s1 17 */ Uint32 uNumBytes) {
	/* v1 3 */ _TagFileMapElementT *pTag;
	/* v1 3 */ Int32 iOffset;
}

/* 00233178 000000ac */ TagErrorE TagFileGetNumber(/* s1 17 */ TagFileDescriptorT *pDescriptor, /* s2 18 */ TagT Tag, /* s0 16 */ Uint32 *pOccurence) {
	/* v1 3 */ _TagFileMapElementT *pCursor;
	/* v1 3 */ _TagFileMapElementT *pFirst;
	/* a0 4 */ _TagFileMapElementT *pLast;
}

/* 00233228 00000094 */ TagErrorE TagFileGetTagSize(/* s1 17 */ TagFileDescriptorT *pDescriptor, /* s2 18 */ TagT Tag, /* s3 19 */ Uint32 uOccurence, /* s0 16 */ Uint32 *pNumBytes) {
	/* s4 20 */ TagErrorE eError;
	/* v0 2 */ _TagFileMapElementT *pTag;
}

/* 002332c0 000000b4 */ TagErrorE TagFileGetStorageMediaInfo(/* a0 4 */ TagFileDescriptorT *pDescriptor) {}

/* 00233378 00000398 */ TagErrorE TagFileProcess(/* s1 17 */ SFIOProcessE *pProcess, /* s3 19 */ Int32 *pResult) {
	/* 0x0(sp) */ SFIOSignatureT FileID;
	/* s0 16 */ TagErrorE eError;
	/* s0 16 */ SFIOErrorE eSFIOError;
	/* v0 2 */ CryptErrorE eCryptError;
	/* s1 17 */ TagBufferT *pReservedBuffer;
}

/* 00233710 00000044 */ TagErrorE TagFileSetSystemData(/* s0 16 */ SFIOSystemDataT *pData) {}

/* 00233758 000000ec */ TagErrorE TagFileAlloc(/* s2 18 */ void **pMem, /* s3 19 */ Uint32 uID, /* s1 17 */ Uint32 uSize, /* s0 16 */ TagModeE eMode) {
	/* 0x0(sp) */ Uint32 uAlignAddr;
	/* 0x4(sp) */ Uint32 uAlignSize;
	/* s0 16 */ Uint32 uTotal;
}

/* 00233848 00000094 */ TagErrorE TagFileFree(/* s2 18 */ void *pMem, /* s3 19 */ Uint32 uID, /* s1 17 */ Uint32 uSize, /* s0 16 */ TagModeE eMode) {
	/* 0x0(sp) */ Uint32 uAlignAddr;
	/* 0x4(sp) */ Uint32 uAlignSize;
	/* v0 2 */ Uint32 uTotal;
}

/* 002338e0 0000003c */ TagErrorE TagFileCalculateDataSize(/* a0 4 */ Uint32 uDataSize, /* s0 16 */ Uint32 *pFinalSize) {}

// *****************************************************************************
// FILE -- D:\packages\EASSDK\dev\source\common\Checksum\ChecksumCRC32.c
// *****************************************************************************

/* 00233920 0000003c */ static Uint32 _ChecksumCRC32_Calculate(/* a0 4 */ Uint32 uValue) {
	/* a1 5 */ Uint8 uLoop;
	/* a0 4 */ Uint32 uCRC;
}

/* 00233960 00000048 */ static void _ChecksumCRC32_CreateLookupTable(/* a0 4 */ Uint32 *pTable) {
	/* s0 16 */ Uint32 uLoop;
}

/* 002339a8 00000090 */ static ChecksumErrorE _ChecksumCRC32_Init(/* s1 17 */ Uint32 uMemID) {}
/* 00233a38 0000004c */ static ChecksumErrorE _ChecksumCRC32_Shutdown() {}
/* 00233a88 00000010 */ static Bool _ChecksumCRC32_IsInitialized() {}
/* 00233a98 0000002c */ static ChecksumErrorE _ChecksumCRC32_Start() {}

/* 00233ac8 00000094 */ static ChecksumErrorE _ChecksumCRC32_CalculateBuffer(/* t2 10 */ Uint8 *pBuffer, /* t1 9 */ Uint32 uLength) {
	/* a2 6 */ Uint32 uIndex;
	/* v0 2 */ Uint32 uGen;
	/* a3 7 */ Uint32 uCRC;
}

/* 00233b60 00000058 */ static ChecksumErrorE _ChecksumCRC32_Stop(/* a3 7 */ ChecksumResultT **ppResult) {
	/* bss 53c204 */ static Uint32 uCRCResult;
}

/* 00233bb8 00000034 */ static ChecksumErrorE _ChecksumCRC32_GetSize(/* a0 4 */ Uint32 *pSize) {}
/* 00233bf0 0000000c */ ChecksumType_t* ChecksumCRC32QueryType() {}

// *****************************************************************************
// FILE -- D:\packages\EASSDK\dev\source\common\Crypt\CryptXOR.c
// *****************************************************************************

/* 00233c00 00000028 */ static CryptErrorE _CryptXOR_Init(/* a0 4 */ Uint32 uMemID) {}
/* 00233c28 0000002c */ static CryptErrorE _CryptXOR_Shutdown() {}
/* 00233c58 00000014 */ static Bool _CryptXOR_IsInitialized() {}
/* 00233c70 00000060 */ static CryptErrorE _CryptXOR_Start(/* a3 7 */ void *pKey, /* a1 5 */ Uint32 uKeySize) {}

/* 00233cd0 000000a4 */ static CryptErrorE _CryptXOR_Encrypt(/* a0 4 */ void *pBuffer, /* a1 5 */ Uint32 uBufferSize) {
	/* t0 8 */ Uint32 uOffset;
}

/* 00233d78 000000a4 */ static CryptErrorE _CryptXOR_Decrypt(/* a0 4 */ void *pBuffer, /* a1 5 */ Uint32 uBufferSize) {
	/* t0 8 */ Uint32 uOffset;
}

/* 00233e20 0000004c */ static CryptErrorE _CryptXOR_Stop() {}
/* 00233e70 0000000c */ CryptType_t* CryptXORQueryType() {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00233e80 00000054 */ void __do_global_dtors() {}
/* 00233ed8 000000b0 */ void __do_global_ctors() {}
/* 00233f88 00000020 */ void __main() {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00233fa8 00000010 */ void __default_terminate() {}
/* 00233fb8 00000024 */ void __terminate() {}
/* 00233fe0 0000002c */ void* __throw_type_match(/* a0 4 */ void *catch_type, /* a1 5 */ void *throw_type, /* s0 16 */ void *obj) {}
/* 00234010 00000008 */ void __empty() {}
/* 00234018 00000054 */ static void* new_eh_context() {}
/* 00234070 00000024 */ void* __get_eh_context() {}
/* 00234098 00000028 */ void** __get_eh_info() {}
/* 002340c0 00000028 */ static eh_context* eh_context_initialize() {}
/* 002340e8 00000068 */ static eh_context* eh_context_static() {}
/* 00234150 00000028 */ void*** __get_dynamic_handler_chain() {}
/* 00234178 00000178 */ void __sjthrow() {}
/* 002342f0 00000118 */ void __sjpopnthrow() {}
/* 00234408 00000048 */ int __eh_rtime_match(/* s0 16 */ void *rtime) {}
/* 00234450 00000008 */ short int __get_eh_table_version(/* a0 4 */ exception_descriptor *table) {}
/* 00234458 00000008 */ short int __get_eh_table_language(/* a0 4 */ exception_descriptor *table) {}
/* 00234460 000000d8 */ static void* old_find_exception_handler(/* a0 4 */ void *pc, /* t5 13 */ old_exception_table *table) {}
/* 00234538 00000144 */ static void* find_exception_handler(/* s1 17 */ void *pc, /* s5 21 */ exception_descriptor *table, /* s4 20 */ __eh_info *eh_info, /* a3 7 */ int rethrow, /* s6 22 */ int *cleanup) {}
/* 00234680 00000088 */ static word_type* get_reg_addr(/* a2 6 */ unsigned int reg, /* a1 5 */ frame_state *udata, /* a2 6 */ frame_state *sub_udata) {}
/* 00234708 0000007c */ static void copy_reg(/* s1 17 */ unsigned int reg, /* a1 5 */ frame_state *udata, /* s0 16 */ frame_state *target_udata) {}
/* 00234788 0000008c */ static void* next_stack_level(/* a0 4 */ void *pc, /* s1 17 */ frame_state *udata, /* s0 16 */ frame_state *caller_udata) {}
/* 00234818 00000008 */ void __unwinding_cleanup() {}
/* 00234820 00000330 */ static void* throw_helper(/* s3 19 */ eh_context *eh, /* s0 16 */ void *pc, /* fp 30 */ frame_state *my_udata, /* 0x5e4(sp) */ frame_state **udata_p) {}
/* 00234b50 000001dc */ void __throw() {}
/* 00234d30 000001e4 */ void __rethrow(/* 0x5e4(sp) */ void *index) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00234f18 00000030 */ void exit(/* s0 16 */ int status) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00234f48 000000f4 */ DItype __fixunssfdi(/* f12 50 */ SFtype original_a) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00235040 00000098 */ DFtype __floatdidf(/* s0 16 */ DItype u) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 002350d8 00000060 */ DItype __muldi3(/* a0 4 */ DItype u, /* a1 5 */ DItype v) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00235138 00000008 */ void __pure_virtual() {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00235140 000005d0 */ UDItype __udivdi3(/* a0 4 */ UDItype n, /* a1 5 */ UDItype d) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/build/gcc/dp-bit.c
// *****************************************************************************

/* 00235710 0000012c */ FLO_type __pack_d(/* a0 4 */ fp_number_type *src) {}
/* 00235840 0000009c */ void __unpack_d(/* a0 4 */ FLO_union_type *src, /* a1 5 */ fp_number_type *dst) {}
/* 002358e0 00000240 */ static fp_number_type* _fpadd_parts(/* t0 8 */ fp_number_type *a, /* a1 5 */ fp_number_type *b, /* a2 6 */ fp_number_type *tmp) {}
/* 00235b20 00000058 */ FLO_type dpadd(/* 0x60(sp) */ FLO_type arg_a, /* 0x68(sp) */ FLO_type arg_b) {}
/* 00235b78 00000064 */ FLO_type dpsub(/* 0x60(sp) */ FLO_type arg_a, /* 0x68(sp) */ FLO_type arg_b) {}
/* 00235be0 000002a8 */ FLO_type dpmul(/* 0x60(sp) */ FLO_type arg_a, /* 0x68(sp) */ FLO_type arg_b) {}
/* 00235e88 00000168 */ FLO_type dpdiv(/* 0x40(sp) */ FLO_type arg_a, /* 0x48(sp) */ FLO_type arg_b) {}
/* 00235ff0 00000114 */ int __fpcmp_parts_d(/* a0 4 */ fp_number_type *a, /* a1 5 */ fp_number_type *b) {}
/* 00236108 0000004c */ SItype dpcmp(/* 0x40(sp) */ FLO_type arg_a, /* 0x48(sp) */ FLO_type arg_b) {}
/* 00236158 000000b8 */ FLO_type litodp(/* a0 4 */ SItype arg_a) {}
/* 00236210 00000094 */ SItype dptoli(/* 0x20(sp) */ FLO_type arg_a) {}
/* 002362a8 000000a0 */ USItype dptoul(/* 0x20(sp) */ FLO_type arg_a) {}
/* 00236348 00000038 */ FLO_type __negdf2(/* 0x20(sp) */ FLO_type arg_a) {}
/* 00236380 0000002c */ DFtype __make_dp(/* a0 4 */ fp_class_type class, /* a1 5 */ unsigned int sign, /* a2 6 */ int exp, /* a3 7 */ UDItype frac) {}
/* 002363b0 00000054 */ SFtype dptofp(/* 0x20(sp) */ DFtype arg_a) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/cp/exception.cc
// *****************************************************************************

/* 00236408 00000018 */ void terminate() {}
/* 00236420 00000010 */ void __default_unexpected() {}

/* 00236430 00000010 */ terminate_handler set_terminate(/* a0 4 */ terminate_handler func) {
	/* v0 2 */ terminate_handler old;
}

/* 00236440 00000010 */ unexpected_handler set_unexpected(/* a0 4 */ unexpected_handler func) {
	/* v0 2 */ unexpected_handler old;
}

/* 00236450 00000018 */ void unexpected() {}
/* 00236468 00000024 */ void* __cp_exception_info() {}
/* 00236490 00000020 */ cp_eh_info* __cp_eh_info() {}

/* 002364b0 00000038 */ cp_eh_info* __start_cp_handler() {
	/* v1 3 */ cp_eh_info *p;
}

/* 002364e8 00000028 */ void* __eh_alloc(/* a0 4 */ size_t size) {
	/* v0 2 */ void *p;
}

/* 00236510 0000001c */ void __eh_free(/* a0 4 */ void *p) {}

/* 00236530 0000006c */ void* __cplus_type_matcher(/* s0 16 */ cp_eh_info *info, /* a1 5 */ rtimetype match_info, /* a2 6 */ exception_descriptor *exception_table) {
	/* v0 2 */ void *ret;
}

/* 002365a0 00000090 */ void __cp_push_exception(/* s3 19 */ void *value, /* s2 18 */ void *type, /* s1 17 */ void (*cleanup)(/* parameters unknown */)) {
	/* s0 16 */ cp_eh_info *p;
	/* v0 2 */ cp_eh_info **q;
}

/* 00236630 000000c4 */ void __cp_pop_exception(/* s0 16 */ cp_eh_info *p) {
	/* a0 4 */ cp_eh_info **q;
}

/* 002366f8 00000030 */ void __uncatch_exception() {
	/* v0 2 */ cp_eh_info *p;
}

/* 00236728 000001b4 */ void __check_eh_spec(/* s3 19 */ int n, /* s5 21 */ void **spec) {
	/* s2 18 */ cp_eh_info *p;
	/* s0 16 */ int i;
	/* s4 20 */ cp_eh_info *__exception_info;
	/* s1 17 */ type_info &bad_exc;
	/* s0 16 */ int i;
	/* s0 16 */ int i;
}

/* 002368e0 00000080 */ void __throw_bad_cast() {}
/* 00236960 00000080 */ void __throw_bad_typeid() {}

/* 002369e0 00000034 */ bool uncaught_exception() {
	/* v0 2 */ cp_eh_info *p;
}

/* 00236a18 00000028 */ char* exception::what(/* a0 4 */ exception *this) {}

/* 00236a40 00000030 */ void bad_exception::~bad_exception(/* a0 4 */ bad_exception *this, /* a1 5 */ int __in_chrg) {
	/* a0 4 */ exception *this;
}

/* 00236a70 00000014 */ bad_exception* bad_exception::bad_exception(/* a0 4 */ bad_exception *this) {
	/* a0 4 */ exception *this;
	/* a0 4 */ exception *this;
}

/* 00236a88 00000050 */ type_info& bad_exception type_info function() {}
/* 00236ad8 00000030 */ void exception::~exception(/* a0 4 */ exception *this, /* a1 5 */ int __in_chrg) {}
/* 00236b08 00000014 */ exception* exception::exception(/* a0 4 */ exception *this) {}
/* 00236b20 00000040 */ type_info& exception type_info function() {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/build/gcc/fp-bit.c
// *****************************************************************************

/* 00236b60 0000010c */ FLO_type __pack_f(/* a0 4 */ fp_number_type *src) {}
/* 00236c70 00000090 */ void __unpack_f(/* a0 4 */ FLO_union_type *src, /* a1 5 */ fp_number_type *dst) {}
/* 00236d00 00000234 */ static fp_number_type* _fpadd_parts(/* t1 9 */ fp_number_type *a, /* a1 5 */ fp_number_type *b, /* a2 6 */ fp_number_type *tmp) {}
/* 00236f38 00000058 */ FLO_type fpadd(/* 0x30(sp) */ FLO_type arg_a, /* 0x34(sp) */ FLO_type arg_b) {}
/* 00236f90 00000064 */ FLO_type fpsub(/* 0x30(sp) */ FLO_type arg_a, /* 0x34(sp) */ FLO_type arg_b) {}
/* 00236ff8 000001f4 */ FLO_type fpmul(/* 0x30(sp) */ FLO_type arg_a, /* 0x34(sp) */ FLO_type arg_b) {}
/* 002371f0 00000160 */ FLO_type fpdiv(/* 0x20(sp) */ FLO_type arg_a, /* 0x24(sp) */ FLO_type arg_b) {}
/* 00237350 00000114 */ int __fpcmp_parts_f(/* a0 4 */ fp_number_type *a, /* a1 5 */ fp_number_type *b) {}
/* 00237468 0000004c */ SItype fpcmp(/* 0x20(sp) */ FLO_type arg_a, /* 0x24(sp) */ FLO_type arg_b) {}
/* 002374b8 000000b8 */ FLO_type sitofp(/* a0 4 */ SItype arg_a) {}
/* 00237570 0000008c */ SItype fptosi(/* 0x10(sp) */ FLO_type arg_a) {}
/* 00237600 00000098 */ USItype fptoui(/* 0x10(sp) */ FLO_type arg_a) {}
/* 00237698 00000038 */ FLO_type __negsf2(/* 0x10(sp) */ FLO_type arg_a) {}
/* 002376d0 0000002c */ SFtype __make_fp(/* a0 4 */ fp_class_type class, /* a1 5 */ unsigned int sign, /* a2 6 */ int exp, /* a3 7 */ USItype frac) {}
/* 00237700 00000040 */ DFtype fptodp(/* 0x10(sp) */ SFtype arg_a) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/frame.c
// *****************************************************************************

/* 00237740 00000044 */ static void* decode_uleb128(/* a0 4 */ unsigned char *buf, /* a1 5 */ unsigned int *r) {}
/* 00237788 00000050 */ static void* decode_sleb128(/* a0 4 */ unsigned char *buf, /* a1 5 */ int *r) {}
/* 002377d8 000000fc */ static void fde_merge(/* s1 17 */ fde_vector *v1, /* a1 5 */ fde_vector *v2) {}
/* 002378d8 00000264 */ static fde** end_fde_sort(/* s1 17 */ fde_accumulator *accu, /* a1 5 */ size_t count) {}
/* 00237b40 00000048 */ static size_t count_fdes(/* a0 4 */ fde *this_fde) {}
/* 00237b88 00000088 */ static void add_fdes(/* t0 8 */ fde *this_fde, /* t3 11 */ fde_accumulator *accu, /* a2 6 */ void **beg_ptr, /* a3 7 */ void **end_ptr) {}
/* 00237c10 0000013c */ static void frame_init(/* s3 19 */ object *ob) {}
/* 00237d50 000000e4 */ static fde* find_fde(/* s1 17 */ void *pc) {}
/* 00237e38 00000104 */ static void* extract_cie_info(/* v0 2 */ fde *f, /* s1 17 */ cie_info *c) {}
/* 00237f40 00000398 */ static void* execute_cfa_insn(/* s0 16 */ void *p, /* s1 17 */ frame_state_internal *state, /* s2 18 */ cie_info *info, /* a3 7 */ void **pc) {}
/* 002382d8 00000028 */ void __register_frame_info(/* a0 4 */ void *begin, /* a1 5 */ object *ob) {}
/* 00238300 00000030 */ void __register_frame(/* s0 16 */ void *begin) {}
/* 00238330 00000028 */ void __register_frame_info_table(/* a0 4 */ void *begin, /* a1 5 */ object *ob) {}
/* 00238358 00000030 */ void __register_frame_table(/* s0 16 */ void *begin) {}
/* 00238388 00000078 */ void* __deregister_frame_info(/* a0 4 */ void *begin) {}
/* 00238400 00000020 */ void __deregister_frame(/* a0 4 */ void *begin) {}
/* 00238420 000001d8 */ frame_state* __frame_state_for(/* s6 22 */ void *pc_target, /* s7 23 */ frame_state *state_in) {}
/* 002385f8 00000174 */ static void fde_split(/* t7 15 */ fde_vector *linear, /* t4 12 */ fde_vector *erratic) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/cp/tinfo.cc
// *****************************************************************************

/* 00238770 00000030 */ void type_info::~type_info(/* a0 4 */ type_info *this, /* a1 5 */ int __in_chrg) {}

/* 002387a0 00000040 */ bool type_info::operator==(/* a0 4 */ type_info *this, /* a1 5 */ type_info &arg) {
	/* a0 4 */ type_info *this;
	/* a1 5 */ type_info *this;
}

/* 002387e0 00000024 */ void __rtti_class(/* a0 4 */ void *addr, /* a1 5 */ char *name, /* a2 6 */ base_info *bl, /* a3 7 */ size_t bn) {
	/* a0 4 */ void *place;
	/* a1 5 */ char *name;
	/* a2 6 */ base_info *bl;
	/* a3 7 */ size_t bn;
	/* a1 5 */ char *n;
	/* a1 5 */ char *n;
}

/* 00238808 00000020 */ void __rtti_si(/* a0 4 */ void *addr, /* a1 5 */ char *n, /* a2 6 */ type_info *ti) {
	/* a0 4 */ void *place;
	/* a1 5 */ char *n;
	/* a2 6 */ __user_type_info &b;
	/* a1 5 */ char *n;
	/* a1 5 */ char *n;
}

/* 00238828 0000001c */ void __rtti_user(/* a0 4 */ void *addr, /* a1 5 */ char *name) {
	/* a0 4 */ void *place;
	/* a1 5 */ char *n;
	/* a1 5 */ char *n;
}

/* 00238848 0000002c */ void* __user_type_info::dcast(/* a0 4 */ __user_type_info *this, /* a1 5 */ type_info &to, /* s0 16 */ void *addr) {}
/* 00238878 00000094 */ void* __si_type_info::dcast(/* s0 16 */ __si_type_info *this, /* s1 17 */ type_info &to, /* s3 19 */ int require_public, /* s2 18 */ void *addr, /* s4 20 */ type_info *sub, /* s5 21 */ void *subptr) {}

/* 00238910 000001d8 */ void* __class_type_info::dcast(/* s2 18 */ __class_type_info *this, /* s4 20 */ type_info &desired, /* fp 30 */ int is_public, /* 0x0(sp) */ void *objptr, /* s5 21 */ type_info *sub, /* s7 23 */ void *subptr) {
	/* s3 19 */ void *match_found;
	/* 0x4(sp) */ size_t i;
	/* s0 16 */ void *p;
	/* s1 17 */ void *os;
	/* v0 2 */ void *ns;
}

/* 00238ae8 00000020 */ __class_type_info* __class_type_info::__class_type_info(/* a0 4 */ __class_type_info *this, /* a1 5 */ char *name, /* a2 6 */ base_info *bl, /* a3 7 */ size_t bn) {
	/* a0 4 */ __user_type_info *this;
	/* a0 4 */ type_info *this;
	/* a0 4 */ __user_type_info *this;
}

/* 00238b08 00000050 */ type_info& __class_type_info type_info function() {}
/* 00238b58 0000001c */ void __class_type_info::~__class_type_info(/* a0 4 */ __class_type_info *this, /* a1 5 */ int __in_chrg) {}

/* 00238b78 0000001c */ __si_type_info* __si_type_info::__si_type_info(/* a0 4 */ __si_type_info *this, /* a1 5 */ char *n, /* a2 6 */ __user_type_info &b) {
	/* a0 4 */ __user_type_info *this;
	/* a0 4 */ type_info *this;
	/* a0 4 */ __user_type_info *this;
}

/* 00238b98 00000050 */ type_info& __si_type_info type_info function() {}
/* 00238be8 0000001c */ void __si_type_info::~__si_type_info(/* a0 4 */ __si_type_info *this, /* a1 5 */ int __in_chrg) {}

/* 00238c08 00000018 */ __user_type_info* __user_type_info::__user_type_info(/* v0 2 */ __user_type_info *this, /* a1 5 */ char *n) {
	/* v0 2 */ type_info *this;
}

/* 00238c20 00000050 */ type_info& __user_type_info type_info function() {}
/* 00238c70 0000001c */ void __user_type_info::~__user_type_info(/* a0 4 */ __user_type_info *this, /* a1 5 */ int __in_chrg) {}

/* 00238c90 00000030 */ void bad_typeid::~bad_typeid(/* a0 4 */ bad_typeid *this, /* a1 5 */ int __in_chrg) {
	/* a0 4 */ exception *this;
}

/* 00238cc0 00000014 */ bad_typeid* bad_typeid::bad_typeid(/* a0 4 */ bad_typeid *this) {
	/* a0 4 */ exception *this;
	/* a0 4 */ exception *this;
}

/* 00238cd8 00000050 */ type_info& bad_typeid type_info function() {}

/* 00238d28 00000030 */ void bad_cast::~bad_cast(/* a0 4 */ bad_cast *this, /* a1 5 */ int __in_chrg) {
	/* a0 4 */ exception *this;
}

/* 00238d58 00000014 */ bad_cast* bad_cast::bad_cast(/* a0 4 */ bad_cast *this) {
	/* a0 4 */ exception *this;
	/* a0 4 */ exception *this;
}

/* 00238d70 00000050 */ type_info& bad_cast type_info function() {}
/* 00238dc0 00000020 */ bool type_info::operator!=(/* a0 4 */ type_info *this, /* a1 5 */ type_info &arg) {}
/* 00238de0 00000008 */ char* type_info::name(/* a0 4 */ type_info *this) {}
/* 00238de8 00000018 */ type_info* type_info::type_info(/* v0 2 */ type_info *this, /* a1 5 */ char *n) {}
/* 00238e00 00000040 */ type_info& type_info type_info function() {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/cp/tinfo2.cc
// *****************************************************************************

/* 00238e40 00000024 */ bool type_info::before(/* a0 4 */ type_info *this, /* a1 5 */ type_info &arg) {}

/* 00238e68 000004c8 */ void* __throw_type_match_rtti(/* s1 17 */ void *catch_type_r, /* s0 16 */ void *throw_type_r, /* s6 22 */ void *objptr) {
	/* 0x0(sp) */ void *new_objptr;
	/* t2 10 */ __user_type_info *p;
	/* s0 16 */ __pointer_type_info *fr;
	/* v0 2 */ __pointer_type_info *to;
	/* s0 16 */ type_info *subfr;
	/* s1 17 */ type_info *subto;
	/* s3 19 */ cv cvfrom;
	/* s2 18 */ cv cvto;
	/* v0 2 */ __attr_type_info *at;
	/* v0 2 */ __attr_type_info *at;
	/* t2 10 */ __user_type_info *p;
	/* s0 16 */ __pointer_type_info *pfr;
	/* s1 17 */ __pointer_type_info *pto;
	/* s4 20 */ bool constp;
	/* v0 2 */ __attr_type_info *at;
	/* v0 2 */ __attr_type_info *at;
}

/* 00239330 00000050 */ bool __is_pointer(/* v1 3 */ void *p) {
	/* v0 2 */ __pointer_type_info *pt;
}

/* 00239380 00000020 */ void __rtti_ptr(/* a0 4 */ void *addr, /* a1 5 */ char *n, /* a2 6 */ type_info *ti) {
	/* a0 4 */ void *place;
	/* a1 5 */ char *n;
	/* a2 6 */ type_info &ti;
	/* a1 5 */ char *n;
}

/* 002393a0 00000024 */ void __rtti_attr(/* a0 4 */ void *addr, /* a1 5 */ char *n, /* a2 6 */ int attrval, /* a3 7 */ type_info *ti) {
	/* a0 4 */ void *place;
	/* a1 5 */ char *n;
	/* a2 6 */ cv a;
	/* a3 7 */ type_info &t;
	/* a1 5 */ char *n;
}

/* 002393c8 0000001c */ void __rtti_func(/* a0 4 */ void *addr, /* a1 5 */ char *name) {
	/* a0 4 */ void *place;
	/* a1 5 */ char *n;
	/* a1 5 */ char *n;
}

/* 002393e8 0000001c */ void __rtti_ptmf(/* a0 4 */ void *addr, /* a1 5 */ char *name) {
	/* a0 4 */ void *place;
	/* a1 5 */ char *n;
	/* a1 5 */ char *n;
}

/* 00239408 0000001c */ void __rtti_ptmd(/* a0 4 */ void *addr, /* a1 5 */ char *name) {
	/* a0 4 */ void *place;
	/* a1 5 */ char *n;
	/* a1 5 */ char *n;
}

/* 00239428 0000001c */ void __rtti_array(/* a0 4 */ void *addr, /* a1 5 */ char *name) {
	/* a0 4 */ void *place;
	/* a1 5 */ char *n;
	/* a1 5 */ char *n;
}

/* 00239448 000000ac */ void* __dynamic_cast(/* a0 4 */ type_info& (*from)(/* parameters unknown */), /* s4 20 */ type_info& (*to)(/* parameters unknown */), /* s5 21 */ int require_public, /* s6 22 */ void *address, /* s3 19 */ type_info& (*sub)(/* parameters unknown */), /* s7 23 */ void *subptr) {}
/* 002394f8 00000034 */ type_info& void type_info function() {}
/* 00239530 00000034 */ type_info& long long type_info function() {}
/* 00239568 00000034 */ type_info& long type_info function() {}
/* 002395a0 00000034 */ type_info& int type_info function() {}
/* 002395d8 00000034 */ type_info& short type_info function() {}
/* 00239610 00000034 */ type_info& bool type_info function() {}
/* 00239648 00000034 */ type_info& char type_info function() {}
/* 00239680 00000034 */ type_info& wchar_t type_info function() {}
/* 002396b8 00000034 */ type_info& long double type_info function() {}
/* 002396f0 00000034 */ type_info& double type_info function() {}
/* 00239728 00000034 */ type_info& float type_info function() {}
/* 00239760 00000034 */ type_info& unsigned int type_info function() {}
/* 00239798 00000034 */ type_info& unsigned long type_info function() {}
/* 002397d0 00000034 */ type_info& unsigned long long type_info function() {}
/* 00239808 00000034 */ type_info& unsigned short type_info function() {}
/* 00239840 00000034 */ type_info& unsigned char type_info function() {}
/* 00239878 00000034 */ type_info& signed char type_info function() {}
/* 002398b0 00000050 */ type_info& __array_type_info type_info function() {}
/* 00239900 0000001c */ void __array_type_info::~__array_type_info(/* a0 4 */ __array_type_info *this, /* a1 5 */ int __in_chrg) {}
/* 00239920 00000050 */ type_info& __ptmd_type_info type_info function() {}
/* 00239970 0000001c */ void __ptmd_type_info::~__ptmd_type_info(/* a0 4 */ __ptmd_type_info *this, /* a1 5 */ int __in_chrg) {}
/* 00239990 00000050 */ type_info& __ptmf_type_info type_info function() {}
/* 002399e0 0000001c */ void __ptmf_type_info::~__ptmf_type_info(/* a0 4 */ __ptmf_type_info *this, /* a1 5 */ int __in_chrg) {}
/* 00239a00 00000050 */ type_info& __func_type_info type_info function() {}
/* 00239a50 0000001c */ void __func_type_info::~__func_type_info(/* a0 4 */ __func_type_info *this, /* a1 5 */ int __in_chrg) {}
/* 00239a70 00000050 */ type_info& __builtin_type_info type_info function() {}
/* 00239ac0 0000001c */ void __builtin_type_info::~__builtin_type_info(/* a0 4 */ __builtin_type_info *this, /* a1 5 */ int __in_chrg) {}
/* 00239ae0 00000050 */ type_info& __attr_type_info type_info function() {}
/* 00239b30 0000001c */ void __attr_type_info::~__attr_type_info(/* a0 4 */ __attr_type_info *this, /* a1 5 */ int __in_chrg) {}
/* 00239b50 00000050 */ type_info& __pointer_type_info type_info function() {}
/* 00239ba0 0000001c */ void __pointer_type_info::~__pointer_type_info(/* a0 4 */ __pointer_type_info *this, /* a1 5 */ int __in_chrg) {}

// *****************************************************************************
// FILE -- klib.s
// *****************************************************************************

/* 002450c0 00000010 */ RFU000_FullReset() {}
/* 002450d0 00000010 */ ResetEE() {}
/* 002450e0 00000010 */ SetGsCrt() {}
/* 002450f0 00000010 */ RFU003() {}
/* 00245100 00000010 */ _Exit() {}
/* 00245110 00000010 */ RFU005() {}
/* 00245120 00000010 */ _LoadExecPS2() {}
/* 00245130 00000010 */ _ExecPS2() {}
/* 00245140 00000010 */ RFU008() {}
/* 00245150 00000010 */ RFU009() {}
/* 00245160 00000010 */ AddSbusIntcHandler() {}
/* 00245170 00000010 */ RemoveSbusIntcHandler() {}
/* 00245180 00000010 */ Interrupt2Iop() {}
/* 00245190 00000010 */ SetVTLBRefillHandler() {}
/* 002451a0 00000010 */ SetVCommonHandler() {}
/* 002451b0 00000010 */ SetVInterruptHandler() {}
/* 002451c0 00000010 */ AddIntcHandler() {}
/* 002451d0 00000010 */ AddIntcHandler2() {}
/* 002451e0 00000010 */ RemoveIntcHandler() {}
/* 002451f0 00000010 */ AddDmacHandler() {}
/* 00245200 00000010 */ AddDmacHandler2() {}
/* 00245210 00000010 */ RemoveDmacHandler() {}
/* 00245220 00000010 */ _EnableIntc() {}
/* 00245230 00000010 */ _DisableIntc() {}
/* 00245240 00000010 */ _EnableDmac() {}
/* 00245250 00000010 */ _DisableDmac() {}
/* 00245260 00000010 */ SetAlarm() {}
/* 00245270 00000010 */ ReleaseAlarm() {}
/* 00245280 00000010 */ _iEnableIntc() {}
/* 00245290 00000010 */ _iDisableIntc() {}
/* 002452a0 00000010 */ _iEnableDmac() {}
/* 002452b0 00000010 */ _iDisableDmac() {}
/* 002452c0 00000010 */ iSetAlarm() {}
/* 002452d0 00000010 */ iReleaseAlarm() {}
/* 002452e0 00000010 */ CreateThread() {}
/* 002452f0 00000010 */ DeleteThread() {}
/* 00245300 00000010 */ StartThread() {}
/* 00245310 00000010 */ ExitThread() {}
/* 00245320 00000010 */ ExitDeleteThread() {}
/* 00245330 00000010 */ TerminateThread() {}
/* 00245340 00000010 */ iTerminateThread() {}
/* 00245350 00000010 */ DisableDispatchThread() {}
/* 00245360 00000010 */ EnableDispatchThread() {}
/* 00245370 00000010 */ ChangeThreadPriority() {}
/* 00245380 00000010 */ iChangeThreadPriority() {}
/* 00245390 00000010 */ RotateThreadReadyQueue() {}
/* 002453a0 00000010 */ _iRotateThreadReadyQueue() {}
/* 002453b0 00000010 */ ReleaseWaitThread() {}
/* 002453c0 00000010 */ iReleaseWaitThread() {}
/* 002453d0 00000010 */ GetThreadId() {}
/* 002453e0 00000010 */ ReferThreadStatus() {}
/* 002453f0 00000010 */ iReferThreadStatus() {}
/* 00245400 00000010 */ SleepThread() {}
/* 00245410 00000010 */ WakeupThread() {}
/* 00245420 00000010 */ _iWakeupThread() {}
/* 00245430 00000010 */ CancelWakeupThread() {}
/* 00245440 00000010 */ iCancelWakeupThread() {}
/* 00245450 00000010 */ SuspendThread() {}
/* 00245460 00000010 */ _iSuspendThread() {}
/* 00245470 00000010 */ ResumeThread() {}
/* 00245480 00000010 */ iResumeThread() {}
/* 00245490 00000010 */ JoinThread() {}
/* 002454a0 00000010 */ RFU060() {}
/* 002454b0 00000010 */ RFU061() {}
/* 002454c0 00000010 */ EndOfHeap() {}
/* 002454d0 00000010 */ RFU063() {}
/* 002454e0 00000010 */ CreateSema() {}
/* 002454f0 00000010 */ DeleteSema() {}
/* 00245500 00000010 */ SignalSema() {}
/* 00245510 00000010 */ iSignalSema() {}
/* 00245520 00000010 */ WaitSema() {}
/* 00245530 00000010 */ PollSema() {}
/* 00245540 00000010 */ iPollSema() {}
/* 00245550 00000010 */ ReferSemaStatus() {}
/* 00245560 00000010 */ iReferSemaStatus() {}
/* 00245570 00000010 */ RFU073() {}
/* 00245580 00000010 */ SetOsdConfigParam() {}
/* 00245590 00000010 */ GetOsdConfigParam() {}
/* 002455a0 00000010 */ GetGsHParam() {}
/* 002455b0 00000010 */ GetGsVParam() {}
/* 002455c0 00000010 */ SetGsHParam() {}
/* 002455d0 00000010 */ SetGsVParam() {}
/* 002455e0 00000010 */ RFU080_CreateEventFlag() {}
/* 002455f0 00000010 */ RFU081_DeleteEventFlag() {}
/* 00245600 00000010 */ RFU082_SetEventFlag() {}
/* 00245610 00000010 */ RFU083_iSetEventFlag() {}
/* 00245620 00000010 */ RFU084_ClearEventFlag() {}
/* 00245630 00000010 */ RFU085_iClearEventFlag() {}
/* 00245640 00000010 */ RFU086_WaitEvnetFlag() {}
/* 00245650 00000010 */ RFU087_PollEvnetFlag() {}
/* 00245660 00000010 */ RFU088_iPollEvnetFlag() {}
/* 00245670 00000010 */ RFU089_ReferEventFlagStatus() {}
/* 00245680 00000010 */ RFU090_iReferEventFlagStatus() {}
/* 00245690 00000010 */ RFU091() {}
/* 002456a0 00000010 */ EnableIntcHandler() {}
/* 002456b0 00000010 */ iEnableIntcHandler() {}
/* 002456c0 00000010 */ DisableIntcHandler() {}
/* 002456d0 00000010 */ iDisableIntcHandler() {}
/* 002456e0 00000010 */ EnableDmacHandler() {}
/* 002456f0 00000010 */ iEnableDmacHandler() {}
/* 00245700 00000010 */ DisableDmacHandler() {}
/* 00245710 00000010 */ iDisableDmacHandler() {}
/* 00245720 00000010 */ KSeg0() {}
/* 00245730 00000010 */ EnableCache() {}
/* 00245740 00000010 */ DisableCache() {}
/* 00245750 00000010 */ GetCop0() {}
/* 00245760 00000010 */ FlushCache() {}
/* 00245770 00000010 */ CpuConfig() {}
/* 00245780 00000010 */ iGetCop0() {}
/* 00245790 00000010 */ iFlushCache() {}
/* 002457a0 00000010 */ iCpuConfig() {}
/* 002457b0 00000010 */ sceSifStopDma() {}
/* 002457c0 00000010 */ SetCPUTimerHandler() {}
/* 002457d0 00000010 */ SetCPUTimer() {}
/* 002457e0 00000010 */ SetOsdConfigParam2() {}
/* 002457f0 00000010 */ GetOsdConfigParam2() {}
/* 00245800 00000010 */ GsGetIMR() {}
/* 00245810 00000010 */ iGsGetIMR() {}
/* 00245820 00000010 */ GsPutIMR() {}
/* 00245830 00000010 */ iGsPutIMR() {}
/* 00245840 00000010 */ SetPgifHandler() {}
/* 00245850 00000010 */ SetVSyncFlag() {}
/* 00245860 00000010 */ RFU116() {}
/* 00245870 00000010 */ _print() {}
/* 00245880 00000010 */ sceSifDmaStat() {}
/* 00245890 00000010 */ isceSifDmaStat() {}
/* 002458a0 00000010 */ sceSifSetDma() {}
/* 002458b0 00000010 */ isceSifSetDma() {}
/* 002458c0 00000010 */ sceSifSetDChain() {}
/* 002458d0 00000010 */ isceSifSetDChain() {}
/* 002458e0 00000010 */ sceSifSetReg() {}
/* 002458f0 00000010 */ sceSifGetReg() {}
/* 00245900 00000010 */ _ExecOSD() {}
/* 00245910 00000010 */ Deci2Call() {}
/* 00245920 00000010 */ PSMode() {}
/* 00245930 00000010 */ MachineType() {}
/* 00245940 00000010 */ GetMemorySize() {}
/* 00245950 00000010 */ _InitTLB() {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/glue.c
// *****************************************************************************

/* 00245960 0000000c */ void sceResetttyinit() {}
/* 00245970 00000090 */ void VSync() {}

/* 00245a00 000000a4 */ long int VSync2() {
	/* 0x0(sp) */ int flag;
	/* 0x8(sp) */ long int csr;
}

/* 00245aa8 0000007c */ int write(/* a0 4 */ int fd, /* s1 17 */ char *buf, /* s2 18 */ int nbytes) {}
/* 00245b28 00000074 */ int read(/* a0 4 */ int fd, /* s1 17 */ char *buf, /* s2 18 */ int nbytes) {}
/* 00245ba0 00000028 */ int open(/* a0 4 */ char *buf, /* a1 5 */ int flags, /* a2 6 */ int mode) {}
/* 00245bc8 00000008 */ int close(/* a0 4 */ int fd) {}
/* 00245bd0 00000008 */ int ioctl(/* a0 4 */ int fd, /* a1 5 */ int cmd, /* a2 6 */ int arg) {}
/* 00245bd8 00000008 */ int lseek(/* a0 4 */ int fd, /* a1 5 */ int offset, /* a2 6 */ int whence) {}

/* 00245be0 000000ac */ caddr_t sbrk(/* a0 4 */ int nbytes) {
	/* data 2b3bbc */ static caddr_t heap_ptr;
	/* v0 2 */ caddr_t base;
	/* s0 16 */ caddr_t ptr;
	/* v0 2 */ unsigned int stat;
	/* s1 17 */ unsigned int oldstat;
}

/* 00245c90 00000008 */ int isatty(/* a0 4 */ int fd) {}
/* 00245c98 00000014 */ int fstat(/* a0 4 */ int fd, /* a1 5 */ stat *buf) {}
/* 00245cb0 00000008 */ int getpid() {}
/* 00245cb8 00000028 */ int kill(/* a0 4 */ int pid, /* a1 5 */ int sig) {}
/* 00245ce0 00000028 */ int stat(/* a0 4 */ char *path, /* a1 5 */ stat *buf) {}
/* 00245d08 00000028 */ int unlink(/* a0 4 */ char *path) {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/cache.c
// *****************************************************************************

/* 00245d30 000000a4 */ void _sceSDC(/* a0 4 */ u_int x, /* a1 5 */ u_int y) {
	/* v0 2 */ u_int z;
	/* v0 2 */ u_int t;
	/* a2 6 */ int i;
}

/* 00245dd8 0000007c */ void SyncDCache(/* s2 18 */ void *p, /* s1 17 */ void *q) {
	/* s0 16 */ u_int stat;
}

/* 00245e58 00000014 */ void iSyncDCache(/* a0 4 */ void *p, /* a1 5 */ void *q) {}

/* 00245e70 000000a4 */ void _sceIDC(/* a0 4 */ u_int x, /* a1 5 */ u_int y) {
	/* v0 2 */ u_int z;
	/* v0 2 */ u_int t;
	/* a2 6 */ int i;
}

/* 00245f18 0000007c */ void InvalidDCache(/* s2 18 */ void *p, /* s1 17 */ void *q) {
	/* s0 16 */ u_int stat;
}

/* 00245f98 00000014 */ void iInvalidDCache(/* a0 4 */ void *p, /* a1 5 */ void *q) {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/intr.c
// *****************************************************************************

/* 00245fb0 00000010 */ int QueryIntrContext() {
	/* v0 2 */ int status;
}

/* 00245fc0 00000068 */ int DisableIntc(/* s1 17 */ int n) {
	/* s0 16 */ u_int stat;
	/* s1 17 */ int rv;
}

/* 00246028 00000068 */ int EnableIntc(/* s1 17 */ int n) {
	/* s0 16 */ u_int stat;
	/* s1 17 */ int rv;
}

/* 00246090 00000068 */ int DisableDmac(/* s1 17 */ int n) {
	/* s0 16 */ u_int stat;
	/* s1 17 */ int rv;
}

/* 002460f8 00000068 */ int EnableDmac(/* s1 17 */ int n) {
	/* s0 16 */ u_int stat;
	/* s1 17 */ int rv;
}

/* 00246160 00000020 */ int iEnableIntc(/* a0 4 */ int n) {}
/* 00246180 00000020 */ int iDisableIntc(/* a0 4 */ int n) {}
/* 002461a0 00000020 */ int iEnableDmac(/* a0 4 */ int n) {}
/* 002461c0 00000020 */ int iDisableDmac(/* a0 4 */ int n) {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/thread.c
// *****************************************************************************

/* 002461e0 000000d8 */ static void topThread(/* a0 4 */ void *arg) {
	/* s1 17 */ rqueue *r;
	/* v1 3 */ int idx;
}

/* 002462b8 000000d4 */ int InitThread() {
	/* 0x0(sp) */ ThreadParam tp;
	/* 0x30(sp) */ SemaParam sp;
	/* bss 53c410 */ static char stack[1024];
}

/* 00246390 00000094 */ int iWakeupThread(/* a0 4 */ int tid) {
	/* v0 2 */ int idx;
	/* s0 16 */ int val;
}

/* 00246428 0000007c */ int iRotateThreadReadyQueue(/* s0 16 */ int prio) {
	/* v0 2 */ int idx;
}

/* 002464a8 00000098 */ int iSuspendThread(/* a0 4 */ int tid) {
	/* v0 2 */ int idx;
	/* s0 16 */ int val;
}

/* 00246540 000000c8 */ int DelayThread(/* s1 17 */ u_int uiUsec) {
	/* 0x0(sp) */ SemaParam sp;
	/* s0 16 */ int sid;
	/* s1 17 */ int aid;
	/* v0 2 */ int status;
}

/* 00246608 00000028 */ static u_long _DelayThreadHandler(/* a0 4 */ int id, /* a1 5 */ u_long ulSchedule, /* a2 6 */ u_long ulActual, /* a3 7 */ void *arg, /* t0 8 */ void *addr) {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/tty.c
// *****************************************************************************

/* 00246630 00000024 */ static CHQUEUE* QueueInit(/* a0 4 */ int num) {
	/* bss 53cc40 */ static CHQUEUE q;
}

/* 00246658 0000003c */ static void QueuePeekWriteDone(/* a1 5 */ CHQUEUE *q) {}
/* 00246698 0000003c */ static void QueuePeekReadDone(/* a1 5 */ CHQUEUE *q) {}

/* 002466d8 00000194 */ static void sceTtyHandler(/* a0 4 */ int event, /* s0 16 */ int param, /* s1 17 */ TTYINFO *ti) {
	/* s0 16 */ int param;
	/* s1 17 */ TTYINFO *ti;
	/* s0 16 */ int n;
	/* s2 18 */ sceDeci2Hdr *deci;
	/* s1 17 */ TTYINFO *ti;
	/* a1 5 */ int sz;
}

/* 00246870 00000174 */ int sceTtyWrite(/* s0 16 */ char *buf, /* s2 18 */ int nbytes) {
	/* s4 20 */ int ret;
	/* s1 17 */ int out;
	/* a0 4 */ char *tmp;
	/* s6 22 */ int intr;
}

/* 002469e8 000000d0 */ int sceTtyRead(/* s4 20 */ char *buf, /* s2 18 */ int nbytes) {
	/* v1 3 */ int i;
}

/* 00246ab8 000000bc */ int sceTtyInit() {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/kprintf.c
// *****************************************************************************

/* 00246b78 0000003c */ static int kputchar(/* a0 4 */ int c) {}
/* 00246bb8 000000b0 */ static void deci2Putchar(/* s0 16 */ int c) {}
/* 00246c68 00000034 */ static void serialPutchar(/* a0 4 */ int c) {}

/* 00246ca0 00000094 */ static int ftoi(/* a1 5 */ long unsigned int u) {
	/* a2 6 */ long int exp;
	/* v1 3 */ long int i;
}

/* 00246d38 00000168 */ static void printfloat(/* s0 16 */ double d) {
	/* s1 17 */ int exp;
}

/* 00246ea0 000005f4 */ void _printf(/* s0 16 */ char *fmt, /* s3 19 */ va_list ap) {
	/* 0x0(sp) */ char buf[32];
	/* s0 16 */ char *cp;
	/* s4 20 */ char *zp;
	/* v1 3 */ char ch0;
	/* a3 7 */ char q;
	/* v1 3 */ char *args;
	/* f12 50 */ float argf;
	/* s1 17 */ long int argi;
	/* s1 17 */ long unsigned int argu;
	/* a1 5 */ int n;
	/* s6 22 */ int ret;
}

/* 00247498 00000038 */ void kprintf(/* a0 4 */ char *fmt) {}

/* 002474d0 00000060 */ void scePrintf(/* a0 4 */ char *fmt) {
	/* s1 17 */ void (*save)(/* parameters unknown */);
}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/sifcmd.c
// *****************************************************************************

/* 00247530 0000001c */ static void _set_sreg(/* a0 4 */ void *send, /* a1 5 */ void *local) {}
/* 00247550 0000000c */ static void _change_addr(/* a0 4 */ void *send, /* a1 5 */ void *local) {}
/* 00247560 00000018 */ unsigned int sceSifGetSreg(/* a0 4 */ int rno) {}
/* 00247578 0000001c */ unsigned int sceSifSetSreg(/* a0 4 */ int rno, /* a1 5 */ unsigned int value) {}
/* 00247598 0000000c */ void* sceSifGetDataTable() {}

/* 002475a8 00000280 */ void sceSifInitCmd() {
	/* s0 16 */ int i;
}

/* 00247828 00000034 */ void sceSifExitCmd() {}

/* 00247860 00000018 */ sceSifCmdData* sceSifSetCmdBuffer(/* a0 4 */ sceSifCmdData *p, /* a1 5 */ int size) {
	/* v0 2 */ sceSifCmdData *ret;
}

/* 00247878 00000018 */ sceSifCmdData* sceSifSetSysCmdBuffer(/* a0 4 */ sceSifCmdData *p, /* a1 5 */ int size) {
	/* v0 2 */ sceSifCmdData *ret;
}

/* 00247890 00000078 */ void sceSifAddCmdHandler(/* v0 2 */ unsigned int i, /* a3 7 */ sceSifCmdHandler cf, /* a2 6 */ void *data) {
	/* a1 5 */ void *gp;
	/* a1 5 */ void *gp;
}

/* 00247908 0000004c */ void sceSifRemoveCmdHandler(/* a2 6 */ unsigned int i) {}

/* 00247958 00000134 */ static unsigned int _sceSifSendCmd(/* s4 20 */ unsigned int fcode, /* s3 19 */ unsigned int mode, /* s0 16 */ void *pp, /* s1 17 */ int ps, /* a0 4 */ void *src, /* t1 9 */ void *dest, /* a1 5 */ int size) {
	/* s2 18 */ int cnt;
	/* v0 2 */ unsigned int did;
	/* 0x0(sp) */ sceSifDmaData dmad[2];
}

/* 00247a90 0000003c */ unsigned int sceSifSendCmd(/* a0 4 */ unsigned int fid, /* a1 5 */ void *pp, /* v0 2 */ int ps, /* v1 3 */ void *src, /* t3 11 */ void *dest, /* t2 10 */ int size) {}
/* 00247ad0 0000003c */ unsigned int isceSifSendCmd(/* a0 4 */ unsigned int fid, /* a1 5 */ void *pp, /* v0 2 */ int ps, /* v1 3 */ void *src, /* t3 11 */ void *dest, /* t2 10 */ int size) {}

/* 00247b10 00000148 */ static int _sceSifCmdIntrHdlr(/* a0 4 */ int chan) {
	/* a2 6 */ u_long128 *p;
	/* 0x0(sp) */ u_long128 buffer[7];
	/* s1 17 */ sceSifCmdDataTable *pScdt;
	/* a3 7 */ sceSifCmdHdr *pSch;
	/* a2 6 */ sceSifCmdHandler f;
	/* a0 4 */ int i;
	/* a1 5 */ int s;
	/* v1 3 */ void *newgp;
	/* s0 16 */ void *oldgp;
	/* v0 2 */ void *oldgp;
	/* v1 3 */ void *newgp;
	/* s0 16 */ void *oldgp;
	/* v0 2 */ void *oldgp;
}

/* 00247c58 000000b0 */ void sceSifWriteBackDCache(/* a0 4 */ void *addr, /* a1 5 */ int size) {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/sifrpc.c
// *****************************************************************************

/* 00247d08 0000019c */ void sceSifInitRpc(/* a0 4 */ unsigned int mode) {
	/* a1 5 */ sceSifCmdSRData *p;
}

/* 00247ea8 00000024 */ void sceSifExitRpc() {}

/* 00247ed0 000000a8 */ static _sceRpcPacket* _sceRpcGetPacket(/* s1 17 */ _sceRpcDataTable *pSrdt) {
	/* s0 16 */ _sceRpcPacket *p;
	/* v1 3 */ int i;
}

/* 00247f78 0000001c */ static void _sceRpcFreePacket(/* a0 4 */ _sceRpcPacket *pSrp) {}
/* 00247f98 00000030 */ static _sceRpcPacket* _sceRpcGetFPacket(/* a0 4 */ _sceRpcDataTable *pSrdt) {}
/* 00247fc8 00000040 */ static _sceRpcPacket* _sceRpcGetFPacket2(/* a0 4 */ _sceRpcDataTable *pSrdt, /* a1 5 */ int n) {}

/* 00248008 000000cc */ static void _request_end(/* a0 4 */ void *send, /* a1 5 */ void *local) {
	/* s2 18 */ _sceRpcPacket *p;
	/* v0 2 */ void *newgp;
	/* s0 16 */ void *oldgp;
	/* v1 3 */ void *oldgp;
}

/* 002480d8 00000040 */ static long unsigned int _alarm_rdata(/* a0 4 */ int id, /* a1 5 */ long unsigned int settime, /* a2 6 */ long unsigned int calltime, /* a1 5 */ void *p, /* t0 8 */ void *addr) {}

/* 00248118 000000d0 */ static void _request_rdata(/* s1 17 */ void *send, /* a1 5 */ void *local) {
	/* a0 4 */ _sceRpcDataTable *pSrdt;
	/* s0 16 */ _sceRpcPacket *q;
}

/* 002481e8 00000158 */ int sceSifGetOtherData(/* s1 17 */ sceSifReceiveData *rd, /* s3 19 */ void *src, /* s4 20 */ void *dest, /* s5 21 */ int size, /* s2 18 */ unsigned int mode) {
	/* s0 16 */ _sceRpcPacket *p;
	/* 0x0(sp) */ SemaParam sem;
}

/* 00248340 0000004c */ static sceSifServeData* _search_svdata(/* a0 4 */ unsigned int c, /* a1 5 */ _sceRpcDataTable *t) {
	/* a1 5 */ sceSifQueueData *q;
	/* v1 3 */ sceSifServeData *r;
}

/* 00248390 00000040 */ static long unsigned int _alarm_bind(/* a0 4 */ int id, /* a1 5 */ long unsigned int settime, /* a2 6 */ long unsigned int calltime, /* a3 7 */ void *p, /* t0 8 */ void *addr) {}

/* 002483d0 000000cc */ static void _request_bind(/* s0 16 */ void *send, /* s1 17 */ void *local) {
	/* s2 18 */ _sceRpcPacket *q;
	/* v0 2 */ sceSifServeData *pSsd;
}

/* 002484a0 00000140 */ int sceSifBindRpc(/* s1 17 */ sceSifClientData *bd, /* s3 19 */ unsigned int command, /* s2 18 */ unsigned int mode) {
	/* s0 16 */ _sceRpcPacket *p;
	/* 0x0(sp) */ SemaParam sem;
}

/* 002485e0 0000008c */ static void _request_call(/* a0 4 */ void *send, /* a1 5 */ void *local) {
	/* a0 4 */ _sceRpcPacket *p;
	/* a1 5 */ sceSifServeData *pSsd;
	/* a2 6 */ sceSifQueueData *pSqd;
}

/* 00248670 000001f4 */ int sceSifCallRpc(/* s1 17 */ sceSifClientData *bd, /* s6 22 */ unsigned int fno, /* fp 30 */ unsigned int mode, /* s5 21 */ void *send, /* s2 18 */ int ssize, /* s4 20 */ void *receive, /* s3 19 */ int rsize, /* s7 23 */ sceSifEndFunc func, /* 0xc0(sp) */ void *para) {
	/* v0 2 */ void *para;
	/* s0 16 */ _sceRpcPacket *p;
	/* v0 2 */ void *gp;
	/* 0x0(sp) */ SemaParam sem;
}

/* 00248868 0000003c */ int sceSifCheckStatRpc(/* a0 4 */ sceSifRpcData *cd) {}

/* 002488a8 00000094 */ void sceSifSetRpcQueue(/* s1 17 */ sceSifQueueData *pSrqd, /* s0 16 */ int key) {
	/* a0 4 */ sceSifQueueData *q;
}

/* 00248940 000000d0 */ void sceSifRegisterRpc(/* s5 21 */ sceSifServeData *pr, /* s0 16 */ unsigned int commnad, /* s1 17 */ sceSifRpcFunc func, /* s2 18 */ void *buff, /* s3 19 */ sceSifRpcFunc cfunc, /* s4 20 */ void *cbuff, /* s6 22 */ sceSifQueueData *pq) {
	/* a0 4 */ sceSifServeData *r;
}

/* 00248a10 00000094 */ sceSifServeData* sceSifRemoveRpc(/* s1 17 */ sceSifServeData *pr, /* s2 18 */ sceSifQueueData *pq) {
	/* s0 16 */ sceSifServeData *r;
}

/* 00248aa8 00000090 */ sceSifQueueData* sceSifRemoveRpcQueue(/* s1 17 */ sceSifQueueData *pSrqd) {
	/* s0 16 */ sceSifQueueData *q;
}

/* 00248b38 00000058 */ sceSifServeData* sceSifGetNextRequest(/* s0 16 */ sceSifQueueData *qd) {
	/* s1 17 */ sceSifServeData *rd;
}

/* 00248b90 000001c4 */ void sceSifExecRequest(/* s1 17 */ sceSifServeData *rdp) {
	/* s0 16 */ _sceRpcPacket *p;
	/* s2 18 */ void *ret;
	/* s4 20 */ int rsize;
	/* v0 2 */ unsigned int id;
	/* s3 19 */ int cnt;
	/* 0x0(sp) */ sceSifDmaData data[2];
	/* v1 3 */ int j;
}

/* 00248d58 00000040 */ void sceSifRpcLoop(/* s0 16 */ sceSifQueueData *qd) {
	/* v0 2 */ sceSifServeData *rdp;
}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/filestub.c
// *****************************************************************************

/* 00248d98 0000005c */ static void _sceFsIobSemaMK() {
	/* 0x0(sp) */ SemaParam sparam;
}

/* 00248df8 00000088 */ static _sceFsIob* new_iob() {
	/* s0 16 */ _sceFsIob *io;
}

/* 00248e80 0000006c */ static _sceFsIob* get_iob(/* s0 16 */ int fd) {
	/* s0 16 */ _sceFsIob *ret;
}

/* 00248ef0 000003b4 */ static void _sceFs_Rcv_Intr(/* a0 4 */ void *pkt, /* a1 5 */ void *data) {
	/* 0x0(sp) */ int ee_semid;
	/* 0x8(sp) */ unsigned int ee_retadr;
	/* 0xc(sp) */ unsigned int ee_retsiz;
	/* 0x4(sp) */ unsigned int ee_retmod;
	/* a2 6 */ int i;
	/* t0 8 */ unsigned char *cp;
	/* t2 10 */ unsigned char *uccp;
	/* s0 16 */ unsigned char *rcv_buf;
	/* a3 7 */ _sceFsReadIntrData *ridp;
	/* 0x10(sp) */ unsigned int r_addr;
	/* 0x14(sp) */ unsigned int r_size;
}

/* 002492a8 0000004c */ static void _sceFsSemInit() {
	/* 0x0(sp) */ SemaParam sparam;
}

/* 002492f8 0000002c */ static int _sceFsWaitS(/* a0 4 */ int cur_cmd) {}
/* 00249328 0000000c */ static void _sceFsSigSema() {}

/* 00249338 000000a4 */ int* scePowerOffHandler(/* s3 19 */ void (*func)(/* parameters unknown */), /* s2 18 */ void *addr) {
	/* s1 17 */ int *ret;
	/* v0 2 */ int istat;
	/* s4 20 */ _sceFsPoffData *pd;
}

/* 002493e0 00000044 */ static void _sceFs_Poff_Intr(/* a0 4 */ void *pkt, /* a1 5 */ void *data) {
	/* s0 16 */ void *oldgp;
}

/* 00249428 00000204 */ int sceFsInit() {
	/* v0 2 */ int i;
	/* s0 16 */ int istat;
	/* 0x10(sp) */ int bufmode;
	/* v1 3 */ _sceFsIob *io;
	/* s1 17 */ _sceFsPoffData *pd;
	/* bss 53ebc0 */ static unsigned int rcv_adr[2];
}

/* 00249630 0000008c */ static int _fs_version() {
	/* s3 19 */ char *libver;
}

/* 002496c0 00000034 */ int sceFsReset() {}

/* 002496f8 00000284 */ int sceOpen(/* s0 16 */ char *name, /* s7 23 */ int flag) {
	/* a3 7 */ unsigned int mode;
	/* a1 5 */ int nsize;
	/* s1 17 */ int ret;
	/* 0x30(sp) */ int retfd;
	/* s1 17 */ _sceFsOpenData *od;
	/* s3 19 */ _sceFsIob *io;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 00249980 0000017c */ int sceClose(/* a0 4 */ int fd) {
	/* s2 18 */ _sceFsCloseData *cd;
	/* s0 16 */ _sceFsIob *io;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_close;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 00249b00 00000238 */ int sceLseek(/* a0 4 */ int fd, /* s2 18 */ int offset, /* s4 20 */ int how) {
	/* s1 17 */ _sceFsLseekData *ld;
	/* s0 16 */ _sceFsIob *io;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_lseek;
	/* a2 6 */ int cnt0;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 00249d38 00000260 */ int sceRead(/* a0 4 */ int fd, /* s6 22 */ void *addr, /* s7 23 */ int size) {
	/* s1 17 */ _sceFsReadData *rd;
	/* s0 16 */ _sceFsIob *io;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_read;
	/* a2 6 */ int cnt0;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 00249f98 000002c0 */ int sceWrite(/* a0 4 */ int fd, /* s1 17 */ void *addr, /* s5 21 */ int size) {
	/* s2 18 */ _sceFsWriteData *wd;
	/* s0 16 */ _sceFsIob *io;
	/* a1 5 */ int i;
	/* a2 6 */ int cnt0;
	/* s0 16 */ int psize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_write;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024a258 00000374 */ int sceIoctl(/* a0 4 */ int fd, /* s1 17 */ int cmd, /* s2 18 */ void *arg) {
	/* s3 19 */ _sceFsIoctlData *id;
	/* s0 16 */ _sceFsIob *io;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_ioctl;
	/* a0 4 */ int cnt0;
	/* s5 21 */ int wait;
	/* 0x10(sp) */ SemaParam sparam;
	/* bss 53ebc8 */ static int *ip0;
}

/* 0024a5d0 000001dc */ int sceIoctl2(/* a0 4 */ int fd, /* s5 21 */ int cmd, /* s0 16 */ void *arg, /* s3 19 */ size_t arglen, /* s7 23 */ void *bufp, /* s4 20 */ size_t buflen) {
	/* s1 17 */ _sceFsIoctlData *id;
	/* s2 18 */ _sceFsIob *io;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_ioctl;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024a7b0 000001a4 */ static int _sceCallCode(/* s1 17 */ char *name, /* s6 22 */ unsigned int callcode) {
	/* s2 18 */ _sceFsNameData *cc;
	/* s0 16 */ int nsize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_code;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024a958 0000001c */ int sceRemove(/* a0 4 */ char *name) {}

/* 0024a978 000001ac */ int sceMkdir(/* s1 17 */ char *name, /* s6 22 */ int flag) {
	/* s2 18 */ _sceFsMkdirData *mkd;
	/* s0 16 */ int nsize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_mkdir;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024ab28 0000001c */ int sceRmdir(/* a0 4 */ char *name) {}

/* 0024ab48 0000026c */ int sceFormat(/* s0 16 */ char *path, /* s2 18 */ char *blkdevname, /* s6 22 */ void *arg, /* s1 17 */ int arglen) {
	/* a1 5 */ int nsize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_format;
	/* s3 19 */ _sceFsFormatData *fd;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024adb8 00000120 */ int sceAddDrv(/* s1 17 */ void *addr) {
	/* s0 16 */ _sceFsAddrData *id;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_adddrv;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024aed8 0000001c */ int sceDelDrv(/* a0 4 */ char *name) {}

/* 0024aef8 000000c8 */ int sceDopen(/* s0 16 */ char *name) {
	/* s2 18 */ _sceFsIob *io;
	/* s1 17 */ int ret;
}

/* 0024afc0 00000168 */ int sceDclose(/* a0 4 */ int fd) {
	/* s2 18 */ _sceFsCloseData *cd;
	/* s0 16 */ _sceFsIob *io;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_dclose;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024b128 00000158 */ int sceDread(/* a0 4 */ int fd, /* s1 17 */ sce_dirent *dp) {
	/* s2 18 */ _sceFsReadData *rd;
	/* s0 16 */ _sceFsIob *io;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_dread;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024b280 000001a0 */ int sceGetstat(/* s1 17 */ char *name, /* s6 22 */ sce_stat *dp) {
	/* s2 18 */ _sceFsGStatData *sd;
	/* s0 16 */ int nsize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_getstat;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024b420 0000023c */ int sceChstat(/* s2 18 */ char *name, /* s0 16 */ sce_stat *dp, /* s7 23 */ unsigned int cbit) {
	/* s3 19 */ _sceFsCStatData *cd;
	/* s1 17 */ int nsize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_chstat;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024b660 000001f0 */ int sceRename(/* s1 17 */ char *oldname, /* s0 16 */ char *newname) {
	/* s2 18 */ _sceFsRenameData *rd;
	/* a1 5 */ int nsize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_rename;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024b850 0000001c */ int sceChdir(/* a0 4 */ char *name) {}

/* 0024b870 00000198 */ int sceSync(/* s1 17 */ char *path, /* s5 21 */ int flag) {
	/* s0 16 */ _sceFsSyncData *sd;
	/* a1 5 */ int nsize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_sync;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024ba08 0000026c */ int sceMount(/* s0 16 */ char *fsdevname, /* s3 19 */ char *blkdevname, /* s7 23 */ int flag, /* s6 22 */ void *arg, /* s1 17 */ int arglen) {
	/* s2 18 */ _sceFsMountData *md;
	/* a1 5 */ int nsize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_mount;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024bc78 0000001c */ int sceUmount(/* a0 4 */ char *name) {}

/* 0024bc98 00000238 */ long int sceLseek64(/* a0 4 */ int fd, /* s2 18 */ long int offset, /* s4 20 */ int how) {
	/* s1 17 */ _sceFsLseek64Data *ld;
	/* s0 16 */ _sceFsIob *io;
	/* a2 6 */ int cnt0;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ long int ret_lseek64;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024bed0 00000238 */ int sceDevctl(/* s1 17 */ char *path, /* s7 23 */ int cmd, /* s4 20 */ void *arg, /* s0 16 */ size_t arglen, /* fp 30 */ void *bufp, /* s6 22 */ size_t buflen) {
	/* s2 18 */ _sceFsDevctlData *dd;
	/* a1 5 */ int nsize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_devctl;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024c108 000001e0 */ int sceSymlink(/* s0 16 */ char *existing, /* s1 17 */ char *new) {
	/* s2 18 */ _sceFsSymlinkData *rd;
	/* a1 5 */ int nsize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_link;
	/* 0x10(sp) */ SemaParam sparam;
}

/* 0024c2e8 000001c8 */ int sceReadlink(/* s1 17 */ char *path, /* s5 21 */ char *buf, /* s0 16 */ size_t bufsize) {
	/* s2 18 */ _sceFsReadlinkData *rd;
	/* a1 5 */ int nsize;
	/* s0 16 */ int ret;
	/* 0x30(sp) */ int ret_link;
	/* 0x10(sp) */ SemaParam sparam;
}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/iopheap.c
// *****************************************************************************

/* 0024c4b0 00000088 */ int sceSifInitIopHeap() {
	/* v0 2 */ int i;
}

/* 0024c538 00000070 */ void* sceSifAllocIopHeap(/* a1 5 */ unsigned int size) {}

/* 0024c5a8 00000080 */ void* sceSifAllocSysMemory(/* a3 7 */ int type, /* a1 5 */ unsigned int size, /* a2 6 */ void *addr) {
	/* v1 3 */ _sceAllocSMData *p;
}

/* 0024c628 00000078 */ int sceSifFreeSysMemory(/* a1 5 */ void *addr) {}
/* 0024c6a0 0000001c */ int sceSifFreeIopHeap(/* a0 4 */ void *addr) {}

/* 0024c6c0 000000f0 */ int sceSifLoadIopHeap(/* a2 6 */ char *name, /* a1 5 */ void *addr) {
	/* t0 8 */ int nsize;
}

/* 0024c7b0 00000070 */ unsigned int sceSifQueryMemSize() {}
/* 0024c820 00000070 */ unsigned int sceSifQueryMaxFreeMemSize() {}
/* 0024c890 00000070 */ unsigned int sceSifQueryTotalFreeMemSize() {}
/* 0024c900 0000007c */ void* sceSifQueryBlockTopAddress(/* a1 5 */ void *addr) {}
/* 0024c980 0000007c */ unsigned int sceSifQueryBlockSize(/* a1 5 */ void *addr) {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/eeloadfile.c
// *****************************************************************************

/* 0024ca00 00000100 */ static int _lf_bind() {
	/* v0 2 */ int i;
}

/* 0024cb00 0000008c */ static int _lf_version() {
	/* s3 19 */ char *libver;
}

/* 0024cb90 00000038 */ int sceSifLoadFileReset() {}

/* 0024cbc8 00000208 */ static int _sceSifLoadModuleBuffer(/* s3 19 */ void *iopaddress, /* s1 17 */ int args, /* s0 16 */ char *argp, /* s6 22 */ int *result) {
	/* v0 2 */ int ret;
}

/* 0024cdd0 00000208 */ int sceSifStopModule(/* s3 19 */ int modid, /* s1 17 */ int args, /* s0 16 */ char *argp, /* s6 22 */ int *result) {
	/* v0 2 */ int ret;
}

/* 0024cfd8 00000090 */ int sceSifUnloadModule(/* s1 17 */ int modid) {}
/* 0024d068 000000a0 */ int sceSifSearchModuleByName(/* s0 16 */ char *modulename) {}
/* 0024d108 00000090 */ int sceSifSearchModuleByAddress(/* s1 17 */ void *addr) {}

/* 0024d198 0000001c */ int sceSifLoadModuleBuffer(/* a0 4 */ void *addr, /* a1 5 */ int args, /* a2 6 */ char *argp) {
	/* 0x0(sp) */ int r;
}

/* 0024d1b8 0000001c */ int sceSifLoadStartModuleBuffer(/* a0 4 */ void *addr, /* a1 5 */ int args, /* a2 6 */ char *argp, /* a3 7 */ int *result) {}

/* 0024d1d8 00000224 */ static int _sceSifLoadModule(/* s1 17 */ char *filename, /* s2 18 */ int args, /* s0 16 */ char *argp, /* s4 20 */ int *result, /* s3 19 */ unsigned int fno) {
	/* v0 2 */ int ret;
}

/* 0024d400 00000020 */ int sceSifLoadModule(/* a0 4 */ char *filename, /* a1 5 */ int args, /* a2 6 */ char *argp) {
	/* 0x0(sp) */ int r;
}

/* 0024d420 0000001c */ int sceSifLoadStartModule(/* a0 4 */ char *filename, /* a1 5 */ int args, /* a2 6 */ char *argp, /* a3 7 */ int *result) {}
/* 0024d440 00000104 */ static int _sceSifLoadElfPart(/* s0 16 */ char *filename, /* s2 18 */ char *secname, /* s4 20 */ sceExecData *data, /* s3 19 */ unsigned int fno) {}
/* 0024d548 0000001c */ int sceSifLoadElfPart(/* a0 4 */ char *filename, /* a1 5 */ char *secname, /* a2 6 */ sceExecData *data) {}
/* 0024d568 00000028 */ int sceSifLoadElf(/* a0 4 */ char *filename, /* a2 6 */ sceExecData *data) {}
/* 0024d590 000000ec */ int sceSifGetIopAddr(/* s3 19 */ void *addr, /* s2 18 */ void *val, /* s0 16 */ unsigned int type) {}
/* 0024d680 000000dc */ int sceSifSetIopAddr(/* s2 18 */ void *addr, /* s1 17 */ void *val, /* s0 16 */ unsigned int type) {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/iopreset.c
// *****************************************************************************

/* 0024d760 00000154 */ int sceSifResetIop(/* s1 17 */ char *arg, /* s0 16 */ int flag) {
	/* 0x0(sp) */ sceSifDmaData dmad;
	/* t3 11 */ unsigned int addr;
	/* t1 9 */ int i;
}

/* 0024d8b8 00000028 */ int sceSifIsAliveIop() {}
/* 0024d8e0 00000050 */ int sceSifSyncIop() {}

/* 0024d930 00000110 */ int sceSifRebootIop(/* s0 16 */ char *fn) {
	/* 0x0(sp) */ char buf[80];
	/* v1 3 */ char *d;
	/* s1 17 */ char *s;
	/* a0 4 */ char *l;
}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/diei.c
// *****************************************************************************

/* 0024da40 00000054 */ int DIntr() {
	/* v0 2 */ u_int stat;
	/* v1 3 */ u_int oldstat;
}

/* 0024da98 00000018 */ int EIntr() {
	/* v0 2 */ u_int oldstat;
}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/initsys.c
// *****************************************************************************

/* 0024dab0 00000048 */ static void supplement_crt0() {
	/* 0x0(sp) */ SemaParam libc_sema;
	/* 0x20(sp) */ SemaParam eh_sema;
}

/* 0024daf8 0000000c */ int __sceKernlGetEhSemaId() {}
/* 0024db08 00000010 */ static int Copy(/* a0 4 */ u_int *dst, /* a1 5 */ u_int *src, /* a2 6 */ u_int size) {}

/* 0024db18 00000038 */ static int kCopy(/* a0 4 */ u_int *dst, /* a1 5 */ u_int *src, /* a2 6 */ u_int size) {
	/* a3 7 */ int i;
}

/* 0024db50 00000040 */ static void* kFindAddress(/* a0 4 */ u_int *start, /* a1 5 */ u_int *end, /* a2 6 */ int value) {}
/* 0024db90 00000010 */ static void* FindAddress(/* a0 4 */ u_int *start, /* a1 5 */ u_int *end, /* a2 6 */ int value) {}

/* 0024dba0 00000034 */ void* GetSystemCallEntry(/* a0 4 */ int no) {
	/* 0x0(sp) */ u_int addr;
}

/* 0024dbd8 000000fc */ static void InitSystemCallTableAddress() {
	/* s3 19 */ int *addr0;
	/* s2 18 */ int *addr1;
}

/* 0024dcd8 00000010 */ static void setup(/* a0 4 */ int n, /* a1 5 */ void (*f)(/* parameters unknown */)) {}
/* 0024dce8 00000054 */ void _InitSys() {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/libosd.c
// *****************************************************************************

/* 0024dd40 00000010 */ static void setup(/* a0 4 */ int n, /* a1 5 */ void (*f)(/* parameters unknown */)) {}
/* 0024dd50 00000010 */ static int Copy(/* a0 4 */ u_int *dst, /* a1 5 */ u_int *src, /* a2 6 */ u_int size) {}

/* 0024dd60 00000038 */ static int kCopy(/* a0 4 */ u_int *dst, /* a1 5 */ u_int *src, /* a2 6 */ u_int size) {
	/* a3 7 */ int i;
}

/* 0024dd98 00000010 */ static void* GetEntryAddress(/* a0 4 */ int id) {}

/* 0024dda8 00000068 */ static int PatchIsNeeded() {
	/* 0x4(sp) */ OsdConfigParam p;
	/* 0x0(sp) */ OsdConfigParam back;
}

/* 0024de10 000000ac */ void InitExecPS2() {
	/* s2 18 */ int i;
}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/exit.c
// *****************************************************************************

/* 0024dec0 00000010 */ static int Copy(/* a0 4 */ char *dst, /* a1 5 */ char *src, /* a2 6 */ u_int size) {}

/* 0024ded0 00000030 */ static int kCopy(/* a0 4 */ char *dst, /* a1 5 */ char *src, /* a2 6 */ u_int size) {
	/* a3 7 */ int i;
}

/* 0024df00 00000010 */ static void setup(/* a0 4 */ int n, /* a1 5 */ void (*f)(/* parameters unknown */)) {}

/* 0024df10 00000118 */ static char* SetArg(/* s0 16 */ char *name, /* s2 18 */ int argc, /* s4 20 */ char **args) {
	/* s2 18 */ int i;
	/* s1 17 */ int len;
	/* s3 19 */ char **ppkArgv;
	/* 0x0(sp) */ char *p;
	/* s5 21 */ char *ret;
}

/* 0024e028 00000008 */ static void TerminateLibrary() {}
/* 0024e030 00000070 */ int ExecPS2(/* s1 17 */ void *addr, /* s2 18 */ void *gp, /* s0 16 */ int argc, /* a3 7 */ char **argv) {}

/* 0024e0a0 00000048 */ void LoadExecPS2(/* a0 4 */ char *name, /* s1 17 */ int argc, /* a2 6 */ char **args) {
	/* s0 16 */ char *kName;
}

/* 0024e0e8 00000028 */ void Exit(/* s0 16 */ int n) {}
/* 0024e110 0000004c */ void ExecOSD(/* s0 16 */ int argc, /* a1 5 */ char **argv) {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/alarm.c
// *****************************************************************************

/* 0024e160 00000010 */ static void setup(/* a0 4 */ int n, /* a1 5 */ void (*f)(/* parameters unknown */)) {}
/* 0024e170 00000010 */ static int Copy(/* a0 4 */ u_int *dst, /* a1 5 */ u_int *src, /* a2 6 */ u_int size) {}

/* 0024e180 00000038 */ static int kCopy(/* a0 4 */ u_int *dst, /* a1 5 */ u_int *src, /* a2 6 */ u_int size) {
	/* a3 7 */ int i;
}

/* 0024e1b8 00000010 */ static void* GetEntryAddress(/* a0 4 */ int id) {}

/* 0024e1c8 000000d4 */ void InitAlarm() {
	/* s2 18 */ int i;
}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/timer.c
// *****************************************************************************

/* 0024e2a0 00000070 */ void SetT2(/* a0 4 */ unsigned int *addr, /* a1 5 */ unsigned int t) {
	/* v0 2 */ unsigned int stat;
	/* v1 3 */ unsigned int oldstat;
}

/* 0024e310 00000010 */ void SetT2_COUNT(/* a1 5 */ unsigned int t) {}
/* 0024e320 00000010 */ void SetT2_MODE(/* a1 5 */ unsigned int t) {}
/* 0024e330 00000010 */ void SetT2_COMP(/* a1 5 */ unsigned int t) {}

/* 0024e340 00000154 */ int InitTimer(/* s4 20 */ u_int uiMode) {
	/* v1 3 */ int hid;
	/* s0 16 */ u_int mode;
	/* s1 17 */ int intr;
	/* v0 2 */ int i;
}

/* 0024e498 000000b8 */ int EndTimer() {
	/* s1 17 */ int intr;
}

/* 0024e550 0000002c */ int GetTimerPreScaleFactor() {}

/* 0024e580 00000090 */ int StartTimerSystemTime() {
	/* v0 2 */ u_int mode;
	/* s0 16 */ int intr;
}

/* 0024e610 0000007c */ int StopTimerSystemTime() {
	/* v0 2 */ u_int mode;
	/* s0 16 */ int intr;
}

/* 0024e690 00000190 */ static void SetNextComp(/* a3 7 */ u_long ulNowTime) {
	/* s0 16 */ u_long a0;
	/* v0 2 */ u_long a1;
	/* v0 2 */ long int lDiff;
	/* s2 18 */ u_int mode;
	/* a2 6 */ COUNTER *pCounter;
}

/* 0024e820 00000084 */ static void InsertAlarm(/* a2 6 */ COUNTER *pCounter) {
	/* t0 8 */ COUNTER *pp;
	/* a1 5 */ COUNTER *pn;
	/* a3 7 */ u_long ulAbsCompare;
	/* v0 2 */ u_long ulTarget;
}

/* 0024e8a8 00000038 */ static COUNTER* UnlinkAlarm(/* a0 4 */ COUNTER *pCounter) {
	/* v1 3 */ COUNTER *pNext;
}

/* 0024e8e0 000002a0 */ static int cbTimerHandler(/* a0 4 */ int ca, /* a1 5 */ void *arg, /* s6 22 */ void *addr) {
	/* s1 17 */ u_long ulAbsTime;
	/* v0 2 */ u_int mode;
	/* s0 16 */ COUNTER *pCounter;
	/* s2 18 */ u_long ulTargetCompare;
	/* a2 6 */ u_long ulTargetTime;
	/* a1 5 */ u_long ulNext;
	/* s2 18 */ COUNTER *pNext;
	/* a2 6 */ u_int low;
	/* s1 17 */ u_int mode;
	/* a0 4 */ u_long ulSystemTime;
	/* a0 4 */ u_int low;
	/* s0 16 */ u_int mode;
	/* v0 2 */ u_long ulSystemTime;
	/* v0 2 */ u_int low;
	/* v1 3 */ u_int mode;
}

/* 0024eb80 0000004c */ u_long iGetTimerSystemTime() {
	/* v0 2 */ u_long ulSystemTime;
}

/* 0024ebd0 0000007c */ u_long GetTimerSystemTime() {
	/* v0 2 */ int intr;
	/* s0 16 */ u_long ulSystemTime;
}

/* 0024ec50 00000070 */ int iAllocTimerCounter() {
	/* a2 6 */ COUNTER *pCounter;
}

/* 0024ecc0 00000048 */ int AllocTimerCounter() {
	/* s0 16 */ int intr;
	/* s1 17 */ int id;
}

/* 0024ed08 000000a0 */ int iFreeTimerCounter(/* a0 4 */ int id) {
	/* s0 16 */ COUNTER *pCounter;
}

/* 0024eda8 0000004c */ int FreeTimerCounter(/* s0 16 */ int id) {
	/* s0 16 */ int res;
	/* s1 17 */ int intr;
}

/* 0024edf8 00000050 */ int iGetTimerUsedUnusedCounters(/* a0 4 */ int *pnUsed, /* a1 5 */ int *pnUnused) {}

/* 0024ee48 0000005c */ int GetTimerUsedUnusedCounters(/* s0 16 */ int *pnUsed, /* s1 17 */ int *pnUnused) {
	/* s0 16 */ int res;
	/* s2 18 */ int intr;
}

/* 0024eea8 000000ac */ int iStartTimerCounter(/* a0 4 */ int id) {
	/* s1 17 */ u_long ulNowTime;
	/* s0 16 */ COUNTER *pCounter;
}

/* 0024ef58 0000004c */ int StartTimerCounter(/* s0 16 */ int id) {
	/* s0 16 */ int res;
	/* s1 17 */ int intr;
}

/* 0024efa8 000000c4 */ int iStopTimerCounter(/* a0 4 */ int id) {
	/* s1 17 */ u_long ulNowTime;
	/* s0 16 */ COUNTER *pCounter;
}

/* 0024f070 0000004c */ int StopTimerCounter(/* s0 16 */ int id) {
	/* s0 16 */ int res;
	/* s1 17 */ int intr;
}

/* 0024f0c0 000000cc */ u_long SetTimerCount(/* s1 17 */ int id, /* s2 18 */ u_long ulNewCount) {
	/* s3 19 */ int intr;
	/* s1 17 */ u_long ulOldCount;
	/* s0 16 */ COUNTER *pCounter;
	/* v0 2 */ u_long ulNowTime;
}

/* 0024f190 00000048 */ u_long iGetTimerBaseTime(/* a0 4 */ int id) {
	/* v0 2 */ u_long ulBaseTime;
	/* a1 5 */ COUNTER *pCounter;
}

/* 0024f1d8 0000004c */ u_long GetTimerBaseTime(/* s0 16 */ int id) {
	/* s1 17 */ int intr;
	/* s0 16 */ u_long ulBaseTime;
}

/* 0024f228 0000006c */ u_long iGetTimerCount(/* a0 4 */ int id) {
	/* s0 16 */ u_long ulCount;
	/* s1 17 */ COUNTER *pCounter;
}

/* 0024f298 0000004c */ u_long GetTimerCount(/* s0 16 */ int id) {
	/* s1 17 */ int intr;
	/* s0 16 */ u_long ulCount;
}

/* 0024f2e8 000000f4 */ int iSetTimerHandler(/* a0 4 */ int id, /* s2 18 */ u_long ulSchedule, /* s1 17 */ u_long (*cbHandler)(/* parameters unknown */), /* s3 19 */ void *arg) {
	/* s0 16 */ COUNTER *pCounter;
}

/* 0024f3e0 0000007c */ int SetTimerHandler(/* s0 16 */ int id, /* s1 17 */ u_long ulSchedule, /* s2 18 */ u_long (*cbHandler)(/* parameters unknown */), /* s3 19 */ void *arg) {
	/* s0 16 */ int res;
	/* s4 20 */ int intr;
}

/* 0024f460 000000a8 */ void TimerBusClock2USec(/* s1 17 */ u_long ulClock, /* s0 16 */ u_int *puiSec, /* s2 18 */ u_int *puiUsec) {
	/* v0 2 */ u_int uiSec;
}

/* 0024f508 00000070 */ u_long TimerUSec2BusClock(/* s0 16 */ u_int uiSec, /* a0 4 */ u_int uiUsec) {}
/* 0024f578 00000054 */ float TimerBusClock2Freq(/* a0 4 */ u_long ulClock) {}
/* 0024f5d0 00000034 */ u_long TimerFreq2BusClock(/* f12 50 */ float fFreq) {}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/timeralarm.c
// *****************************************************************************

/* 0024f608 0000004c */ int _InitAlarm() {
	/* a0 4 */ int i;
}

/* 0024f658 0000005c */ static u_long AlarmHandler(/* a0 4 */ int counterid, /* a1 5 */ u_long ulSchedule, /* a2 6 */ u_long ulActual, /* s0 16 */ void *arg, /* t0 8 */ void *addr) {
	/* v0 2 */ u_long ulNext;
}

/* 0024f6b8 0000012c */ int SetTimerAlarm(/* s6 22 */ u_long ulClock, /* s4 20 */ u_long (*cbHandler)(/* parameters unknown */), /* s5 21 */ void *arg) {
	/* s3 19 */ int intr;
	/* s1 17 */ int counterid;
	/* s0 16 */ ALARM *pAlarm;
}

/* 0024f7e8 000000f0 */ int iSetTimerAlarm(/* s5 21 */ u_long ulClock, /* s3 19 */ u_long (*cbHandler)(/* parameters unknown */), /* s4 20 */ void *arg) {
	/* s1 17 */ int counterid;
	/* s0 16 */ ALARM *pAlarm;
}

/* 0024f8d8 000000a8 */ int ReleaseTimerAlarm(/* s0 16 */ int id) {
	/* s2 18 */ int intr;
	/* s1 17 */ ALARM *pAlarm;
	/* s1 17 */ ALARM *pAlarm;
}

/* 0024f980 00000078 */ int iReleaseTimerAlarm(/* a0 4 */ int id) {
	/* s0 16 */ ALARM *pAlarm;
	/* a0 4 */ int res;
	/* s0 16 */ ALARM *pAlarm;
}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/iopnotify.c
// *****************************************************************************

/* 0024f9f8 00000024 */ void sceSifInitRebootNotify() {}

/* 0024fa20 0000007c */ sceSifRebootNotifyData* sceSifSetRebootNotifyBuffer(/* s0 16 */ sceSifRebootNotifyData *p, /* s1 17 */ int size) {
	/* s3 19 */ sceSifRebootNotifyData *old;
	/* s2 18 */ int intr;
}

/* 0024faa0 000000b8 */ int sceSifAddRebootNotifyHandler(/* s0 16 */ u_int pos, /* s2 18 */ sceSifRebootNotifyHandler pFunc, /* s3 19 */ void *data) {
	/* s1 17 */ sceSifRebootNotifyData *pBuf;
	/* v0 2 */ int sz;
	/* v0 2 */ int intr;
	/* a0 4 */ void *gp;
}

/* 0024fb58 00000054 */ int sceSifRemoveRebootNotifyHandler(/* a0 4 */ u_int pos) {
	/* a1 5 */ sceSifRebootNotifyData *pBuf;
	/* v0 2 */ int sz;
}

/* 0024fbb0 000000a8 */ void sceSifExecNotifyHandler(/* a0 4 */ int sys, /* s3 19 */ int mode) {
	/* s2 18 */ void *oldgp;
	/* v0 2 */ sceSifRebootNotifyData *pBuf;
	/* s1 17 */ int i;
	/* v1 3 */ int sz;
	/* v0 2 */ void *newgp;
	/* v1 3 */ void *oldgp;
	/* s2 18 */ void *newgp;
	/* v0 2 */ void *oldgp;
}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/deci2.c
// *****************************************************************************

/* 0024fc58 00000044 */ int sceDeci2Open(/* a0 4 */ short unsigned int protocol, /* a1 5 */ void *opt, /* a2 6 */ void (*handler)(/* parameters unknown */)) {
	/* 0x0(sp) */ u_int args[4];
}

/* 0024fca0 00000024 */ int sceDeci2Close(/* a0 4 */ int s) {
	/* 0x0(sp) */ u_int args[1];
}

/* 0024fcc8 00000030 */ int sceDeci2ReqSend(/* a0 4 */ int s, /* a1 5 */ char dest) {
	/* 0x0(sp) */ u_int args[2];
}

/* 0024fcf8 00000024 */ void sceDeci2Poll(/* a0 4 */ int s) {
	/* 0x0(sp) */ u_int args[1];
}

/* 0024fd20 00000030 */ int sceDeci2GrpOpen(/* a0 4 */ unsigned char grp, /* a1 5 */ void *opt, /* a2 6 */ int (*handler)(/* parameters unknown */)) {
	/* 0x0(sp) */ u_int args[5];
}

/* 0024fd50 00000024 */ int sceDeci2GrpClose(/* a0 4 */ int g) {
	/* 0x0(sp) */ u_int args[1];
}

/* 0024fd78 00000034 */ int sceDeci2ExRecv(/* a0 4 */ int s, /* v0 2 */ void *buf, /* a2 6 */ u_short len) {
	/* 0x0(sp) */ u_int args[3];
}

/* 0024fdb0 00000034 */ int sceDeci2ExSend(/* a0 4 */ int s, /* v0 2 */ void *buf, /* a2 6 */ u_short len) {
	/* 0x0(sp) */ u_int args[3];
}

/* 0024fde8 00000030 */ int sceDeci2ExReqSend(/* a0 4 */ int s, /* a1 5 */ char dest) {
	/* 0x0(sp) */ u_int args[2];
}

/* 0024fe18 00000024 */ int sceDeci2ExLock(/* a0 4 */ int s) {
	/* 0x0(sp) */ u_int args[1];
}

/* 0024fe40 00000024 */ int sceDeci2ExUnLock(/* a0 4 */ int s) {
	/* 0x0(sp) */ u_int args[1];
}

/* 0024fe68 00000024 */ int kputs(/* a0 4 */ char *str) {
	/* 0x0(sp) */ u_int args[1];
}

// *****************************************************************************
// FILE -- /home/xokano/globe_cvs/g_28/g/ee/src/kernel/libc/tlbfunc.c
// *****************************************************************************

/* 0024fe90 00000060 */ void* SetTLBHandler(/* s1 17 */ void (*h)(/* parameters unknown */)) {}

/* 0024fef0 00000084 */ void* SetDebugHandler(/* a2 6 */ int i, /* a1 5 */ void (*h)(/* parameters unknown */)) {
	/* s0 16 */ void *p;
}

/* 0024ff78 00000010 */ static int Copy(/* a0 4 */ u_int *dst, /* a1 5 */ u_int *src, /* a2 6 */ u_int size) {}

/* 0024ff88 00000038 */ static int kCopy(/* a0 4 */ u_int *dst, /* a1 5 */ u_int *src, /* a2 6 */ u_int size) {
	/* a3 7 */ int i;
}

/* 0024ffc0 00000010 */ static void* GetEntryAddress(/* a0 4 */ int id) {}
/* 0024ffd0 00000010 */ static void setup(/* a0 4 */ int n, /* a1 5 */ void (*f)(/* parameters unknown */)) {}

/* 0024ffe0 000000c4 */ void InitTLBFunctions() {
	/* s2 18 */ int i;
}

/* 002500a8 00000010 */ int PutTLBEntry(/* a0 4 */ u_int mask, /* a1 5 */ u_int hi, /* a2 6 */ u_int lo0, /* a3 7 */ u_int lo1) {}
/* 002500b8 00000010 */ int iPutTLBEntry(/* a0 4 */ u_int mask, /* a1 5 */ u_int hi, /* a2 6 */ u_int lo0, /* a3 7 */ u_int lo1) {}
/* 002500c8 00000010 */ static int _SetTLBEntry(/* a0 4 */ int index, /* a1 5 */ u_int mask, /* a2 6 */ u_int hi, /* a3 7 */ u_int lo0, /* t0 8 */ u_int lo1) {}
/* 002500d8 00000030 */ int SetTLBEntry(/* a0 4 */ int index, /* a1 5 */ u_int mask, /* a2 6 */ u_int hi, /* a3 7 */ u_int lo0, /* t0 8 */ u_int lo1) {}
/* 00250108 00000010 */ int iSetTLBEntry(/* a0 4 */ int index, /* a1 5 */ u_int mask, /* a2 6 */ u_int hi, /* a3 7 */ u_int lo0, /* t0 8 */ u_int lo1) {}
/* 00250118 00000010 */ int GetTLBEntry(/* a0 4 */ int index, /* a1 5 */ u_int *mask, /* a2 6 */ u_int *hi, /* a3 7 */ u_int *lo0, /* t0 8 */ u_int *lo1) {}
/* 00250128 00000010 */ int iGetTLBEntry(/* a0 4 */ int index, /* a1 5 */ u_int *mask, /* a2 6 */ u_int *hi, /* a3 7 */ u_int *lo0, /* t0 8 */ u_int *lo1) {}
/* 00250138 00000010 */ int ProbeTLBEntry(/* a0 4 */ u_int hi, /* a1 5 */ u_int *mask, /* a2 6 */ u_int *lo0, /* a3 7 */ u_int *lo1) {}
/* 00250148 00000010 */ int iProbeTLBEntry(/* a0 4 */ u_int hi, /* a1 5 */ u_int *mask, /* a2 6 */ u_int *lo0, /* a3 7 */ u_int *lo1) {}
/* 00250158 00000010 */ int ExpandScratchPad(/* a0 4 */ u_int page) {}
/* 00250168 00000040 */ int InitTLB() {}

/* 002501a8 000001f4 */ static int InitTLB32MB() {
	/* t9 25 */ int index;
	/* v0 2 */ unsigned int hi;
	/* s1 17 */ int end;
	/* s0 16 */ TLBEntry *tlb;
	/* a1 5 */ int k;
	/* a3 7 */ int d;
	/* t1 9 */ int e;
	/* s0 16 */ int i;
}

// *****************************************************************************
// FILE -- tlbtrap.s
// *****************************************************************************

/* 002503c0 0000011c */ _kTLBException() {}
/* 00250500 000000f4 */ _kExitTLBHandler() {}
/* 00250600 0000011c */ _kDebugException() {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00250720 000006ec */ DItype __divdi3(/* a0 4 */ DItype u, /* a1 5 */ DItype v) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00250e10 0000005c */ DItype __fixdfdi(/* s0 16 */ DFtype a) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00250e70 000000ec */ DItype __fixunsdfdi(/* s1 17 */ DFtype a) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00250f60 000000e0 */ SFtype __floatdisf(/* s1 17 */ DItype u) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 00251040 00000668 */ DItype __moddi3(/* a0 4 */ DItype u, /* a1 5 */ DItype v) {}

// *****************************************************************************
// FILE -- /usr/local/sce/ee/gcc/src/gcc/libgcc2.c
// *****************************************************************************

/* 002516a8 00000540 */ UDItype __umoddi3(/* a0 4 */ UDItype u, /* a1 5 */ UDItype v) {}

// *****************************************************************************
// FILE -- (unknown)
// *****************************************************************************

/* 001b0480 0000006c */ FONT_switchcase(/* parameters unknown */) {}
/* 001b0650 00000038 */ FONT_findkern16(/* parameters unknown */) {}
/* 001b0688 00000044 */ FONT_findkern12(/* parameters unknown */) {}
/* 001b0758 00000008 */ NULL_draw(/* parameters unknown */) {}
/* 001b0828 0000002c */ SHAPE_bpctobpp(/* parameters unknown */) {}
/* 001b0858 0000012c */ SHAPE_imagesize(/* parameters unknown */) {}
/* 001b0988 0000002c */ GetNumberOfColours(/* parameters unknown */) {}
/* 001b15d8 00000088 */ Pixel4Offset(/* parameters unknown */) {}
/* 001b1660 00000084 */ Pixel8Offset(/* parameters unknown */) {}
/* 001b16e8 00000088 */ Pixel16Offset(/* parameters unknown */) {}
/* 001b1770 00000010 */ Pixel32Offset(/* parameters unknown */) {}
/* 001dd980 00000034 */ _RpcVertical(/* parameters unknown */) {}
/* 001dd9b8 0000001c */ _WakeupThread(/* parameters unknown */) {}
/* 001dd9d8 0000054c */ _RpcCall(/* parameters unknown */) {}
/* 001dec08 0000001c */ _NetConnResetHandler(/* parameters unknown */) {}
/* 001dec28 00000058 */ _NetConnResetVblankHandler(/* parameters unknown */) {}
/* 001dec80 000000a0 */ _NetConnShutdownExpansionDevices(/* parameters unknown */) {}
/* 001ded20 0000009c */ _NetConnResetThread(/* parameters unknown */) {}
/* 001df6c0 00000078 */ _LobbyApiInitRequest(/* parameters unknown */) {}
/* 001df738 000000b4 */ _LobbyApiEnqueueRequestCallback(/* parameters unknown */) {}
/* 001df7f0 000000e0 */ _LobbyApiDequeueRequestCallback(/* parameters unknown */) {}
/* 001df8d0 00000040 */ _LobbyApiClearQueue(/* parameters unknown */) {}
/* 001df910 0000004c */ _LobbyApiOneShotCB(/* parameters unknown */) {}
/* 001df960 0000009c */ _LobbyApiSetOneShotCB(/* parameters unknown */) {}
/* 001dfa00 00000058 */ _LobbyApiCancelOneShotCB(/* parameters unknown */) {}
/* 001dfa58 000000f4 */ _LobbyApiPingCallback(/* parameters unknown */) {}
/* 001dfb50 000000fc */ _LobbyApiUpdatePing(/* parameters unknown */) {}
/* 001dfc50 00000064 */ _LobbyApiPingServerCb(/* parameters unknown */) {}
/* 001dfcb8 00000034 */ _LobbyApiPingServer(/* parameters unknown */) {}
/* 001dfcf0 00000018 */ _LobbyApiResetServerIcmpPingState(/* parameters unknown */) {}
/* 001dfd08 00000060 */ _LobbyApiConfigCallback(/* parameters unknown */) {}
/* 001dfd68 000000c8 */ _LobbyApiUpdateInit(/* parameters unknown */) {}
/* 001dfe30 000000bc */ _LobbyApiCheckTimeout(/* parameters unknown */) {}
/* 001dfef0 0000008c */ _LobbyApiDisconnect(/* parameters unknown */) {}
/* 001dff80 00000264 */ _LobbyApiUpdateRdir(/* parameters unknown */) {}
/* 001e2318 00000030 */ _LobbyApiAsyncProcess_ses(/* parameters unknown */) {}
/* 001e1e58 00000030 */ _LobbyApiAsyncProcess_mgm(/* parameters unknown */) {}
/* 001e1eb8 00000068 */ _LobbyApiAsyncProcess_msg(/* parameters unknown */) {}
/* 001e2ee8 00000168 */ _LobbyApiAsyncProcess_who(/* parameters unknown */) {}
/* 001e20d8 0000023c */ _LobbyApiAsyncProcess_rom(/* parameters unknown */) {}
/* 001e1f20 000001b4 */ _LobbyApiAsyncProcess_pop(/* parameters unknown */) {}
/* 001e2888 00000354 */ _LobbyApiAsyncProcess_usr(/* parameters unknown */) {}
/* 001e2be0 000001c4 */ _LobbyApiAsyncProcess_uss(/* parameters unknown */) {}
/* 001e2da8 00000140 */ _LobbyApiAsyncProcess_ust(/* parameters unknown */) {}
/* 001e2630 00000258 */ _LobbyApiAsyncProcess_usm(/* parameters unknown */) {}
/* 001e2348 000001b4 */ _LobbyApiAsyncProcess_snp(/* parameters unknown */) {}
/* 001e19b8 00000224 */ _LobbyApiAsyncProcess_agm(/* parameters unknown */) {}
/* 001e1c10 00000214 */ _LobbyApiAsyncProcess_gam(/* parameters unknown */) {}
/* 001e2500 0000012c */ _LobbyApiAsyncProcess_sst(/* parameters unknown */) {}
/* 001e1be0 00000030 */ _LobbyApiAsyncProcess_bud(/* parameters unknown */) {}
/* 001e1e88 00000030 */ _LobbyApiAsyncProcess_mlg(/* parameters unknown */) {}
/* 001e1e28 00000030 */ _LobbyApiAsyncProcess_koh(/* parameters unknown */) {}
/* 001e1910 000000a4 */ _LobbyApiAsyncProcessRoomChange(/* parameters unknown */) {}
/* 001e3228 0000006c */ _LobbyApiListFlush(/* parameters unknown */) {}
/* 001e3da8 00000060 */ _LobbyApiSynthesizeMachineAddr(/* parameters unknown */) {}
/* 001e5098 00000184 */ DispListExpand(/* parameters unknown */) {}
/* 001e5220 00000034 */ DispListCompare(/* parameters unknown */) {}
/* 001e5b08 0000006c */ HashStrHash(/* parameters unknown */) {}
/* 001e6230 00000080 */ _PingManagerFindRecordByAddress(/* parameters unknown */) {}
/* 001e62b0 00000058 */ _PingManagerFindRecordLRU(/* parameters unknown */) {}
/* 001e6308 0000001c */ _PingManagerInvalidateRecord(/* parameters unknown */) {}
/* 001e6328 00000040 */ _PingManagerUpdateTimer(/* parameters unknown */) {}
/* 001e6368 00000028 */ _PingManagerGetPing(/* parameters unknown */) {}
/* 001e6b38 000000a4 */ _LobbyRegParseNameList(/* parameters unknown */) {}
/* 001e6be0 00000014 */ _LobbyRegSetAlert(/* parameters unknown */) {}
/* 001e6bf8 00000028 */ _LobbyRegSetCallback(/* parameters unknown */) {}
/* 001e6c20 000000a8 */ _LobbyRegSetContext(/* parameters unknown */) {}
/* 001e6f30 00000050 */ _LobbyRegTransitionBack(/* parameters unknown */) {}
/* 001e6cc8 00000038 */ _LobbyRegRequest(/* parameters unknown */) {}
/* 001e6d00 00000108 */ _LobbyRegDoLogin(/* parameters unknown */) {}
/* 001e74f0 000001b4 */ _LobbyRegAuthCallback(/* parameters unknown */) {}
/* 001e6e08 000000cc */ _LobbyRegDoSelPers(/* parameters unknown */) {}
/* 001e7920 00000060 */ _LobbyRegPersCallback(/* parameters unknown */) {}
/* 001e6ed8 00000054 */ _LobbyRegSaveResponse(/* parameters unknown */) {}
/* 001e6f80 00000064 */ _LobbyRegValidateTransition(/* parameters unknown */) {}
/* 001e6fe8 00000068 */ _LobbyRegValidateLength(/* parameters unknown */) {}
/* 001e7050 000000a0 */ _LobbyRegValidateBirth(/* parameters unknown */) {}
/* 001e70f0 00000084 */ _LobbyRegValidateEmail(/* parameters unknown */) {}
/* 001e7178 0000006c */ _LobbyRegValidateGender(/* parameters unknown */) {}
/* 001e71e8 00000088 */ _LobbyRegValidateSpam(/* parameters unknown */) {}
/* 001e7270 0000011c */ _LobbyRegValidateAcct(/* parameters unknown */) {}
/* 001e7390 00000064 */ _LobbyRegSetValidateError(/* parameters unknown */) {}
/* 001e73f8 000000f8 */ _LobbyRegAcctCallback(/* parameters unknown */) {}
/* 001e76a8 000000d0 */ _LobbyRegCperCallback(/* parameters unknown */) {}
/* 001e7778 000000c8 */ _LobbyRegDperCallback(/* parameters unknown */) {}
/* 001e7840 0000006c */ _LobbyRegEditCallback(/* parameters unknown */) {}
/* 001e78b0 00000070 */ _LobbyRegLostCallback(/* parameters unknown */) {}
/* 001e7980 0000005c */ _LobbyRegUserCallback(/* parameters unknown */) {}
/* 001e79e0 000000c8 */ _LobbyRegConnCallback(/* parameters unknown */) {}
/* 001e7aa8 00000098 */ _LobbyRegIdleCallback(/* parameters unknown */) {}
/* 001e8fb8 000003e8 */ _LobbyStatbookViewCb(/* parameters unknown */) {}
/* 001e93a0 00000190 */ _LobbyStatbookViewInfoCb(/* parameters unknown */) {}
/* 001e9530 000001a8 */ _LobbyStatbookRowCb(/* parameters unknown */) {}
/* 001e9c60 00000068 */ _ParseNumber(/* parameters unknown */) {}
/* 001e9cc8 000001cc */ _SecsToTime(/* parameters unknown */) {}
/* 001e9e98 0000010c */ _TimeToSecs(/* parameters unknown */) {}
/* 001e9fa8 00000280 */ _TagFieldSetupAppend(/* parameters unknown */) {}
/* 001ea228 00000030 */ _TagFieldSetupCancel(/* parameters unknown */) {}
/* 001ea258 00000028 */ _TagFieldSetupTerm(/* parameters unknown */) {}
/* 001ed6c0 00000018 */ _LobbySettingCmdDone(/* parameters unknown */) {}
/* 001ed6d8 00000050 */ _LobbySettingSaveCallback(/* parameters unknown */) {}
/* 001ed728 00000070 */ _LobbySettingLoadCallback(/* parameters unknown */) {}
/* 001edad0 00000014 */ _LobbyFindUserResetState(/* parameters unknown */) {}
/* 001edae8 0000029c */ _LobbyFindUserExtractToCache(/* parameters unknown */) {}
/* 001edd88 000000f4 */ _LobbyFindUserUserCallback(/* parameters unknown */) {}
/* 001ede80 00000158 */ _LobbyFindUserOnlnCallback(/* parameters unknown */) {}
/* 001ee818 00000158 */ _preConnTempBudAdd(/* parameters unknown */) {}
/* 001f0228 000000e0 */ _HLBMsgCallback(/* parameters unknown */) {}
/* 001f0308 00000030 */ _HLBBuddyDelMemCallback(/* parameters unknown */) {}
/* 001f0338 000001e0 */ _HLBBuddyChangeCallback(/* parameters unknown */) {}
/* 001f2940 00000090 */ _TourneyApiReqSetup(/* parameters unknown */) {}
/* 001f29d0 00000148 */ _TourneyApiGetGameList(/* parameters unknown */) {}
/* 001f52b0 00000074 */ _TourneyApiFlagGetMaskAndShift(/* parameters unknown */) {}
/* 001f5e78 0000001c */ ProtoAriesRecvCB(/* parameters unknown */) {}
/* 001f62e0 00000028 */ _ProtoHttpDataCB(/* parameters unknown */) {}
/* 001f72e0 00000018 */ ProtoPingResponseCB(/* parameters unknown */) {}
/* 001f7d30 0000019c */ _WebOfferUrl(/* parameters unknown */) {}
/* 001fa1c0 00000070 */ _CheatParseHex(/* parameters unknown */) {}
/* 001fa230 00000058 */ _CheatParseRange(/* parameters unknown */) {}
/* 001fa528 000001d4 */ _SecsToTime(/* parameters unknown */) {}
/* 001fa700 000000e8 */ _LobbyLocalizePickCurrency(/* parameters unknown */) {}
/* 001fa7e8 0000021c */ _LobbyLocalizeFormatTime(/* parameters unknown */) {}
/* 001faa08 000001dc */ _LobbyLocalizeFormatDate(/* parameters unknown */) {}
/* 001fabe8 000001e8 */ _LobbyLocalizeFormatNum(/* parameters unknown */) {}
/* 001fb3f8 00000044 */ _Utf8GetNumBytes(/* parameters unknown */) {}
/* 001fb440 00000098 */ _Utf8DecodeToUCS2(/* parameters unknown */) {}
/* 001fb550 00000094 */ _Utf8Translate(/* parameters unknown */) {}
/* 001fba48 000000ac */ _BuddyApiProd(/* parameters unknown */) {}
/* 001fbaf8 00000008 */ _BuddyApiDebug(/* parameters unknown */) {}
/* 001fbb90 00000078 */ _BuddyApiTransact(/* parameters unknown */) {}
/* 001fbd08 00000058 */ _BuddyApiTruncUsername(/* parameters unknown */) {}
/* 001fbd60 000000ac */ _BuddyApiParseResource(/* parameters unknown */) {}
/* 001fbe10 000001b4 */ _BuddyApiRecv(/* parameters unknown */) {}
/* 001fc388 00000134 */ _BuddyApiPresSend(/* parameters unknown */) {}
/* 001fc4c8 00000058 */ _BuddyApiStatus(/* parameters unknown */) {}
/* 001fc520 000000c0 */ _BuddyApiSetCallback(/* parameters unknown */) {}
/* 001ffbb8 000000e0 */ _SocketLookupDone(/* parameters unknown */) {}
/* 001ffc98 00000088 */ _SocketLookupFree(/* parameters unknown */) {}
/* 001ffd20 000000cc */ _SocketLookup(/* parameters unknown */) {}
/* 001ffe70 00001038 */ _CryptMD5Transform(/* parameters unknown */) {}
/* 0023b200 0000001c */ lflush(/* parameters unknown */) {}
/* 0023b510 00000008 */ eofread(/* parameters unknown */) {}
/* 0023f000 00000108 */ _vfwrite(/* parameters unknown */) {}
/* 002403d0 00000108 */ _vfwrite(/* parameters unknown */) {}
/* 00241410 00000264 */ malloc_extend_top(/* parameters unknown */) {}
/* 002439b0 00000058 */ std(/* parameters unknown */) {}
/* 00257a08 0000001c */ EASO::SharedLoginThreadInterface::Sleep(/* parameters unknown */) {}
/* 00252b38 00000008 */ AptString::GetNumGCPointers(/* parameters unknown */) {}
/* 001e97a0 0000004c */ LobbyStatbookDestroy(/* parameters unknown */) {}
/* 001e3420 000000a0 */ LobbyApiListAlloc(/* parameters unknown */) {}
/* 002564c8 0000016c */ XmlMemberIndex::in_word_set(/* parameters unknown */) {}
/* 00258340 000003d4 */ void FONT_drawtextx<unsigned char>(/* parameters unknown */) {}
/* 001e4c18 000000f0 */ LobbyApiExtractUserSetMember(/* parameters unknown */) {}
/* 00244f58 0000006c */ _mprec_log10(/* parameters unknown */) {}
/* 001e3340 00000090 */ LobbyApiListDestroy(/* parameters unknown */) {}
/* 0025b6a0 00000008 */ EASO::NetLogin::SetLobby(/* parameters unknown */) {}
/* 001f8230 0000003c */ WebOfferResultData(/* parameters unknown */) {}
/* 0023a2e0 00000118 */ strcpy(/* parameters unknown */) {}
/* 001f1148 0000004c */ HLBMsgListGetMsgByIndex(/* parameters unknown */) {}
/* 0025bad0 00000008 */ EASO::NetLobby::GetChatMsgLimit(/* parameters unknown */) {}
/* 001f2c10 0000002c */ TourneyApiDisconnect(/* parameters unknown */) {}
/* 002442e0 00000084 */ _hi0bits(/* parameters unknown */) {}
/* 002521d0 00000008 */ AptMathObj::ContainsNativeHashVirtual(/* parameters unknown */) {}
/* 00257e58 0000001c */ EAGLInternal::GifState::SetTexture(/* parameters unknown */) {}
/* 001f1718 00000028 */ __HLBListMakeBuddyXX(/* parameters unknown */) {}
/* 001ffdf0 0000001c */ ProtoNameAsync(/* parameters unknown */) {}
/* 001f7248 00000094 */ ProtoPingRequestServer(/* parameters unknown */) {}
/* 001f6f88 0000007c */ ProtoMangleStatus2(/* parameters unknown */) {}
/* 002436c0 00000060 */ _lseek_r(/* parameters unknown */) {}
/* 001ee068 00000048 */ LobbyFindUserCancel(/* parameters unknown */) {}
/* 00257cb8 00000024 */ EAGLInternal::DMAAccumulator::operator delete(/* parameters unknown */) {}
/* 001e5b00 00000008 */ HasherCount(/* parameters unknown */) {}
/* 001fe7b0 00000030 */ BuddyApiUserFound(/* parameters unknown */) {}
/* 00256370 000000bc */ EAStringC::UTF8_ReadCharacter(/* parameters unknown */) {}
/* 001f68c8 00000094 */ ProtoHttpGetBuffer(/* parameters unknown */) {}
/* 00252cc8 00000024 */ AptString::operator delete [](/* parameters unknown */) {}
/* 001dd658 00000060 */ SocketInTextGetAddr(/* parameters unknown */) {}
/* 001b07b0 00000074 */ FONT_getcharacter(/* parameters unknown */) {}
/* 00243c38 000000a8 */ __sflags(/* parameters unknown */) {}
/* 001ee428 00000038 */ _signalBuddyListChanged(/* parameters unknown */) {}
/* 00259de8 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>(/* parameters unknown */) {}
/* 001eba88 00000108 */ TagFieldSetBinary7(/* parameters unknown */) {}
/* 00253d80 00000024 */ AptCIH::setInMainInst(/* parameters unknown */) {}
/* 00255f30 00000044 */ AptScriptFunction::operator delete(/* parameters unknown */) {}
/* 001f3148 00000090 */ TourneyApiReqLeave(/* parameters unknown */) {}
/* 001b1780 00000058 */ SHAPE_clut(/* parameters unknown */) {}
/* 001de260 00000094 */ RpcElapsed(/* parameters unknown */) {}
/* 001e8800 00000024 */ LobbyRegGetNameList(/* parameters unknown */) {}
/* 001f4fb8 0000006c */ TourneyApiGetGameWinner(/* parameters unknown */) {}
/* 001f7520 00000068 */ _WebOfferGetColor(/* parameters unknown */) {}
/* 0023e7a0 0000009c */ strtok_r(/* parameters unknown */) {}
/* 00253788 00000048 */ EAStringC::operator==(/* parameters unknown */) {}
/* 001f2180 0000001c */ HLBApiPresenceExtra(/* parameters unknown */) {}
/* 001e37c8 000000c4 */ LobbyApiListDel(/* parameters unknown */) {}
/* 00255000 00000008 */ AptScriptColour::operator new [](/* parameters unknown */) {}
/* 001ee410 00000018 */ _setLastOpState(/* parameters unknown */) {}
/* 00257540 00000044 */ AptXmlNode::operator delete(/* parameters unknown */) {}
/* 001dea58 0000005c */ NetConnIdleAdd(/* parameters unknown */) {}
/* 001b0560 0000002c */ FONTDL_buildw(/* parameters unknown */) {}
/* 00256ec0 00000024 */ AptValue::isNativeFunction(/* parameters unknown */) {}
/* 00202f28 00000000 */ DirtyDnasStop(/* parameters unknown */) {}
/* 001f07a0 00000008 */ HLBApiOverrideMaxMessagesPerBuddy(/* parameters unknown */) {}
/* 001def80 00000390 */ NetConnStatus(/* parameters unknown */) {}
/* 0023b1a0 00000060 */ printf(/* parameters unknown */) {}
/* 00255190 0000000c */ AptNativeHash::IsEmpty(/* parameters unknown */) {}
/* 00254c90 00000008 */ AptFloat::GetFloat(/* parameters unknown */) {}
/* 001f1ec0 00000200 */ HLBListSendMsgToGroup(/* parameters unknown */) {}
/* 001e5830 000000a8 */ HasherCreate(/* parameters unknown */) {}
/* 001fdea8 00000230 */ BuddyApiAdd(/* parameters unknown */) {}
/* 0023c350 00000054 */ vsprintf(/* parameters unknown */) {}
/* 001f9d10 00000180 */ CryptSSC2Init(/* parameters unknown */) {}
/* 001e05b8 0000007c */ LobbyApiResume(/* parameters unknown */) {}
/* 00253d70 0000000c */ AptCIH::getHasClass(/* parameters unknown */) {}
/* 00254f58 0000001c */ AptGlobal::Lookup(/* parameters unknown */) {}
/* 001f0de8 00000014 */ HLBBudIsBlocked(/* parameters unknown */) {}
/* 0025b700 00000118 */ void EASO::AddPair<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >(/* parameters unknown */) {}
/* 001f9b08 0000004c */ ds_stricmp(/* parameters unknown */) {}
/* 00258000 0000001c */ EAGL::Model::ModelShape::TAR_iterator::TAR_iterator(/* parameters unknown */) {}
/* 00257680 00000030 */ EA::Allocator::IAllocator::~IAllocator(/* parameters unknown */) {}
/* 0023b698 00000068 */ __sseek(/* parameters unknown */) {}
/* 001f5b88 0000018c */ ProtoAriesStatus(/* parameters unknown */) {}
/* 00243b60 0000008c */ __sinit(/* parameters unknown */) {}
/* 001eee50 000000d0 */ _HLBRosterCallback(/* parameters unknown */) {}
/* 001e5828 00000008 */ DispListDataGet(/* parameters unknown */) {}
/* 00253138 00000078 */ AptFrameStack::~AptFrameStack(/* parameters unknown */) {}
/* 00254c98 00000024 */ AptFloat::operator new(/* parameters unknown */) {}
/* 001fb4d8 00000078 */ Utf8EncodeFromUCS2CodePt(/* parameters unknown */) {}
/* 001e3500 000000f4 */ LobbyApiListFree(/* parameters unknown */) {}
/* 001f5078 00000238 */ TourneyApiGetNextGame(/* parameters unknown */) {}
/* 0025a800 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>(/* parameters unknown */) {}
/* 00256f10 00000024 */ AptValue::isExtern(/* parameters unknown */) {}
/* 001ed560 0000015c */ TagFieldGetStructureOffsets(/* parameters unknown */) {}
/* 002424b8 000001d0 */ _setlocale_r(/* parameters unknown */) {}
/* 00257bd0 0000001c */ EASO::MyMutex::Unlock(/* parameters unknown */) {}
/* 00257178 00000024 */ AptDisplayListState::operator delete(/* parameters unknown */) {}
/* 00254318 00000008 */ AptCIH::IsGarbageCollected(/* parameters unknown */) {}
/* 002531d0 00000024 */ EAStringC::operator==(/* parameters unknown */) {}
/* 00243a08 00000074 */ __sfmoreglue(/* parameters unknown */) {}
/* 00257958 00000008 */ EASO::SharedLoginThreadInterface::SetStackSize(/* parameters unknown */) {}
/* 00243e28 00000194 */ ungetc(/* parameters unknown */) {}
/* 00257d18 0000003c */ EAGLInternal::PropertyParser::SkipDelims(/* parameters unknown */) {}
/* 001f6b20 00000008 */ ProtoHttpUpdate(/* parameters unknown */) {}
/* 001e3a10 000000f0 */ LobbyApiPopul(/* parameters unknown */) {}
/* 0025baf0 00000008 */ EASO::NetLobby::GetRoomUpdateFreq(/* parameters unknown */) {}
/* 00259818 00000078 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> & EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=<EASO::FifaStringPolicy>(/* parameters unknown */) {}
/* 0023ca48 00000040 */ __malloc_unlock(/* parameters unknown */) {}
/* 00259958 000001a4 */ EASO::String::LexicalCastNamespace::LexicalCastDetail::LexicalCast_To<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >::LexicalCast_From(/* parameters unknown */) {}
/* 00257ad8 00000008 */ EASO::MySharedLoginAllocator::Release(/* parameters unknown */) {}
/* 00252a80 0000001c */ AptString::cpy(/* parameters unknown */) {}
/* 001f0f30 00000064 */ HLBBudGetPresence(/* parameters unknown */) {}
/* 00255118 00000024 */ AptCharacterButtonInst::operator delete(/* parameters unknown */) {}
/* 0023e680 0000007c */ strlwr(/* parameters unknown */) {}
/* 00251c60 00000010 */ AptArray::ConvertAptValue(/* parameters unknown */) {}
/* 001f4b70 00000138 */ TourneyApiGetUserByName(/* parameters unknown */) {}
/* 001f1740 0000007c */ HLBListUnMakeBuddy(/* parameters unknown */) {}
/* 0023d468 00000ef8 */ _strtod_r(/* parameters unknown */) {}
/* 0025bb70 00000050 */ {anonymous}::CFESharedLoginAllocator::Instance(/* parameters unknown */) {}
/* 002525d8 00000008 */ AptMouse::IsGarbageCollected(/* parameters unknown */) {}
/* 00256a60 0000006c */ AptValue::AptValue(/* parameters unknown */) {}
/* 001f2710 00000014 */ _HLBApiGetXXX(/* parameters unknown */) {}
/* 001b0348 00000044 */ FONT_restore(/* parameters unknown */) {}
/* 001f1380 00000064 */ HLBMsgListDelete(/* parameters unknown */) {}
/* 001f0bc0 0000002c */ HLBApiGetLastOpStatus(/* parameters unknown */) {}
/* 00257ae0 00000074 */ EASO::MyMutex::CreateInstance(/* parameters unknown */) {}
/* 001f7588 000000a8 */ _WebOfferGetButton(/* parameters unknown */) {}
/* 001e7c30 000000a0 */ LobbyRegAutomaticCB(/* parameters unknown */) {}
/* 002566a0 00000008 */ AptValue::IsGarbageCollected(/* parameters unknown */) {}
/* 001ec8a0 00000104 */ TagFieldGetDate(/* parameters unknown */) {}
/* 00256100 0000009c */ AptBoolean::AptBoolean(/* parameters unknown */) {}
/* 001e0378 00000058 */ LobbyApiDestroy(/* parameters unknown */) {}
/* 00256778 00000008 */ AptNone::GetGCPointer(/* parameters unknown */) {}
/* 00252190 00000008 */ AptMathObj::AddRef(/* parameters unknown */) {}
/* 001e5698 0000001c */ DispListChange(/* parameters unknown */) {}
/* 00253388 000003e0 */ void EA::String::BasicString<StringAsVectorEncoding<AptFileSavedInputState>, StringAsVectorPolicy>::Insert<AptFileSavedInputState *>(/* parameters unknown */) {}
/* 00239d10 000000fc */ memmove(/* parameters unknown */) {}
/* 00255ef0 0000003c */ AptScriptFunction::operator new(/* parameters unknown */) {}
/* 001e8f80 00000034 */ _LobbyStatbookInitSlotRows(/* parameters unknown */) {}
/* 00253da8 0000000c */ AptCIH::getInMainInst(/* parameters unknown */) {}
/* 001f0890 000000a8 */ HLBApiConnect(/* parameters unknown */) {}
/* 001f0e18 00000014 */ HLBBudIsIWannaBeHisBuddy(/* parameters unknown */) {}
/* 001ffbb0 00000008 */ BuddyApiRefreshTitle(/* parameters unknown */) {}
/* 0023abb8 00000024 */ _cleanup(/* parameters unknown */) {}
/* 001b1068 00000068 */ SHAPE_metalbin(/* parameters unknown */) {}
/* 0023b388 00000090 */ snprintf(/* parameters unknown */) {}
/* 001f2bb8 00000048 */ TourneyApiDestroy(/* parameters unknown */) {}
/* 001f35c8 00000068 */ TourneyApiReqDelete(/* parameters unknown */) {}
/* 00256d18 00000008 */ AptValue::c_textformat(/* parameters unknown */) {}
/* 001e0cf8 00000128 */ LobbyApiInfoPtr(/* parameters unknown */) {}
/* 001f3240 0000013c */ TourneyApiReqSearch(/* parameters unknown */) {}
/* 001f3730 000000e0 */ TourneyApiRequest(/* parameters unknown */) {}
/* 001e0768 000002d4 */ LobbyApiRequestCB(/* parameters unknown */) {}
/* 00243fc0 000000a8 */ _Balloc(/* parameters unknown */) {}
/* 001f38e8 00000b20 */ TourneyApiUpdate(/* parameters unknown */) {}
/* 001de4a8 00000388 */ RpcStartEE(/* parameters unknown */) {}
/* 001e6160 000000d0 */ LobbyNameSubstr(/* parameters unknown */) {}
/* 0025a070 000000cc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString(/* parameters unknown */) {}
/* 002523c0 00000008 */ AptKey::getNativeHashVirtual(/* parameters unknown */) {}
/* 0025b9c8 00000008 */ EASO::NetLobby::GetNewRoomPrefix(/* parameters unknown */) {}
/* 00253eb8 00000008 */ AptCIH::getAnimationInst(/* parameters unknown */) {}
/* 0025baa8 00000008 */ EASO::NetLobby::SetMangleTestTimeout(/* parameters unknown */) {}
/* 002562c8 00000008 */ AptBoolean::GetBool(/* parameters unknown */) {}
/* 002575d8 00000044 */ AptXmlAttributes::operator delete(/* parameters unknown */) {}
/* 00257908 00000008 */ EASO::SharedLoginThreadInterface::CreateInstance(/* parameters unknown */) {}
/* 00252fd0 00000078 */ AptPrototype::~AptPrototype(/* parameters unknown */) {}
/* 00251fa8 0000001c */ AptObject::Lookup(/* parameters unknown */) {}
/* 0025b4e8 00000030 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *(*)>(/* parameters unknown */) {}
/* 001fb828 000000b0 */ Utf8DecodeToUCS2(/* parameters unknown */) {}
/* 001df5d8 00000078 */ NetConnRemoveResetHandler(/* parameters unknown */) {}
/* 002581a8 00000010 */ EAGL::RenderContextExtension::ToggleSingleBuffered(/* parameters unknown */) {}
/* 001de0e0 00000030 */ RpcCall(/* parameters unknown */) {}
/* 00257708 000000a4 */ EASO::MyMutex::Instance(/* parameters unknown */) {}
/* 001f71b0 00000098 */ ProtoPingRequest2(/* parameters unknown */) {}
/* 00239bc0 0000000c */ __errno(/* parameters unknown */) {}
/* 0025ba20 0000001c */ EASO::NetLobby::SetNewsServer(/* parameters unknown */) {}
/* 001fcf28 000000dc */ _BuddyApiDeleteIfNeeded(/* parameters unknown */) {}
/* 001e58d8 00000058 */ HasherDestroy(/* parameters unknown */) {}
/* 001de1e0 00000080 */ RpcSync(/* parameters unknown */) {}
/* 001df650 00000070 */ NetConnCheckReset(/* parameters unknown */) {}
/* 00252f80 00000008 */ AptPrototype::ContainsNativeHashVirtual(/* parameters unknown */) {}
/* 001ec9a8 000001d0 */ TagFieldSetFloat(/* parameters unknown */) {}
/* 001fe108 00000104 */ BuddyApiGameInvite(/* parameters unknown */) {}
/* 00256d48 00000014 */ AptValue::isXmlNode(/* parameters unknown */) {}
/* 001f1240 00000090 */ HLBMsgListGetMsgText(/* parameters unknown */) {}
/* 001f6860 00000068 */ ProtoHttpAbort(/* parameters unknown */) {}
/* 00256be0 00000040 */ AptValue::setGCRoot(/* parameters unknown */) {}
/* 001eab58 000000fc */ TagFieldSetRaw(/* parameters unknown */) {}
/* 001f8ae8 000003b0 */ WebOfferUpdate(/* parameters unknown */) {}
/* 00252460 00000044 */ AptKey::operator delete(/* parameters unknown */) {}
/* 001f12d0 0000008c */ HLBMsgListGetUnreadCount(/* parameters unknown */) {}
/* 00244d18 0000017c */ _d2b(/* parameters unknown */) {}
/* 00244460 00000208 */ _multiply(/* parameters unknown */) {}
/* 0025ab18 00000024 */ EASO::EasoHub::EasoHub(/* parameters unknown */) {}
/* 001f3898 0000004c */ TourneyApiCancel(/* parameters unknown */) {}
/* 001eb148 000000d4 */ TagFieldSetFlags(/* parameters unknown */) {}
/* 00257e38 0000001c */ EAGLInternal::GifState::SetShading(/* parameters unknown */) {}
/* 002530e0 00000008 */ AptFrameStack::IsGarbageCollected(/* parameters unknown */) {}
/* 00243660 0000005c */ _fstat_r(/* parameters unknown */) {}
/* 001f1670 0000007c */ HLBListBlockBuddy(/* parameters unknown */) {}
/* 00251c70 00000008 */ AptArray::length(/* parameters unknown */) {}
/* 00253f98 00000064 */ AptCIH::isButtonInst(/* parameters unknown */) {}
/* 00254dd0 00000008 */ AptGlobal::getNativeHash(/* parameters unknown */) {}
/* 00254e28 0000003c */ AptGlobal::operator new(/* parameters unknown */) {}
/* 001f9b58 00000064 */ ds_strnicmp(/* parameters unknown */) {}
/* 001f0e00 00000014 */ HLBBudIsWannaBeMyBuddy(/* parameters unknown */) {}
/* 0023b720 00000b10 */ __svfscanf(/* parameters unknown */) {}
/* 00255c78 0000009c */ AptInteger::AptInteger(/* parameters unknown */) {}
/* 00254eb0 00000008 */ AptGlobal::operator new [](/* parameters unknown */) {}
/* 001fe478 00000060 */ BuddyApiFind(/* parameters unknown */) {}
/* 001e9b78 000000c4 */ LobbyStatbookFetchViewInfo(/* parameters unknown */) {}
/* 00258078 00000024 */ EAGL::RenderContext::operator new(/* parameters unknown */) {}
/* 001fe8b8 00000008 */ BuddyApiSuspendXDK(/* parameters unknown */) {}
/* 00252200 0000001c */ AptMathObj::GetGCPointer(/* parameters unknown */) {}
/* 00244e98 000000c0 */ _ratio(/* parameters unknown */) {}
/* 001e01e8 0000018c */ LobbyApiCreate(/* parameters unknown */) {}
/* 00252e70 00000008 */ AptRegister::GetGCPointer(/* parameters unknown */) {}
/* 00255f88 0000003c */ AptSound::operator new(/* parameters unknown */) {}
/* 0025bb38 00000008 */ EASO::NetLobby::GetScoreReportScore1(/* parameters unknown */) {}
/* 00254de0 00000008 */ AptGlobal::IsGarbageCollected(/* parameters unknown */) {}
/* 0025bb58 00000008 */ EASO::EasoUtil::GetAwayScore(/* parameters unknown */) {}
/* 002524b8 00000078 */ AptKey::~AptKey(/* parameters unknown */) {}
/* 001e6860 00000150 */ PingManagerPingAddress(/* parameters unknown */) {}
/* 0023c538 000000d4 */ bsearch(/* parameters unknown */) {}
/* 00255168 00000024 */ AptCharacterButtonInst::operator delete [](/* parameters unknown */) {}
/* 0025b920 00000020 */ EASO::NetLobby::setTickerServer(/* parameters unknown */) {}
/* 00257590 00000008 */ AptXmlNode::operator delete [](/* parameters unknown */) {}
/* 001e3ce8 000000c0 */ LobbyApiListUpdate(/* parameters unknown */) {}
/* 001f0b48 00000078 */ HLBApiResume(/* parameters unknown */) {}
/* 002441a8 00000134 */ _s2b(/* parameters unknown */) {}
/* 001ee0d0 000001dc */ LobbyFindUser(/* parameters unknown */) {}
/* 001f6060 0000014c */ ProtoAriesRecv(/* parameters unknown */) {}
/* 001f7ed0 00000088 */ WebOfferCreate2(/* parameters unknown */) {}
/* 00244428 00000038 */ _i2b(/* parameters unknown */) {}
/* 00256348 00000024 */ AptBoolean::operator delete [](/* parameters unknown */) {}
/* 00254f78 0000003c */ AptScriptColour::operator new(/* parameters unknown */) {}
/* 0025afd0 0000006c */ void EASO::AddPair<char *>(/* parameters unknown */) {}
/* 001e54e0 00000014 */ DispListGet(/* parameters unknown */) {}
/* 001f7118 00000094 */ ProtoPingRequest(/* parameters unknown */) {}
/* 00257e98 00000050 */ EAGLInternal::GifState::SetAlphaTestMethod(/* parameters unknown */) {}
/* 0023ca88 000009dc */ qsort(/* parameters unknown */) {}
/* 001f6d08 0000007c */ ProtoMangleConnectSocket(/* parameters unknown */) {}
/* 00256ee8 00000024 */ AptValue::isScriptFunction(/* parameters unknown */) {}
/* 002571a0 00000084 */ AptCharacterShapeInst::~AptCharacterShapeInst(/* parameters unknown */) {}
/* 00255d18 00000110 */ AptInteger::Create(/* parameters unknown */) {}
/* 001f4868 000000d4 */ TourneyApiGetRound(/* parameters unknown */) {}
/* 001f6960 00000094 */ ProtoHttpSetBuffer(/* parameters unknown */) {}
/* 001f14a0 00000034 */ _HLBMsgListDestroy(/* parameters unknown */) {}
/* 001fd660 00000064 */ BuddyApiDestroy(/* parameters unknown */) {}
/* 001ed0a0 00000008 */ TagFieldCmpString(/* parameters unknown */) {}
/* 001f31d8 00000068 */ TourneyApiReqStart(/* parameters unknown */) {}
/* 001b0fc4 00000040 */ SHAPE_name(/* parameters unknown */) {}
/* 001f59f0 00000104 */ ProtoAriesListen(/* parameters unknown */) {}
/* 0025bcd0 00000020 */ bool EASO::String::operator!=<EASO::String::EncodingUtf8, EASO::FifaStringPolicy, EASO::FifaStringPolicy>(/* parameters unknown */) {}
/* 002568f0 0000001c */ AptGlobalExtensionObject::DestroyGCPointers(/* parameters unknown */) {}
/* 00254da8 0000001c */ AptGlobal::DestroyGCPointers(/* parameters unknown */) {}
/* 001f2d60 000001ec */ TourneyApiReqCreate(/* parameters unknown */) {}
/* 001f17c0 00000124 */ HLBListInviteBuddy(/* parameters unknown */) {}
/* 002530f0 00000008 */ AptFrameStack::ContainsNativeHashVirtual(/* parameters unknown */) {}
/* 001de110 000000cc */ RpcBind(/* parameters unknown */) {}
/* 001ed878 0000006c */ LobbySettingSave(/* parameters unknown */) {}
/* 002522b8 00000078 */ AptMathObj::~AptMathObj(/* parameters unknown */) {}
/* 001fea70 0000000c */ BuddyApiRegisterBuddyDelCallback(/* parameters unknown */) {}
/* 00259500 00000314 */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::ConstDbgIterator>(/* parameters unknown */) {}
/* 001f72f8 00000224 */ ProtoPingResponse(/* parameters unknown */) {}
/* 001b06d0 00000088 */ FONT_getkern(/* parameters unknown */) {}
/* 001eaaf0 00000068 */ TagFieldFindBulk(/* parameters unknown */) {}
/* 001ed970 00000058 */ LobbySettingGetStringWithIndex(/* parameters unknown */) {}
/* 00251f80 00000008 */ AptObject::IsGarbageCollected(/* parameters unknown */) {}
/* 001b0390 00000048 */ FONT_init(/* parameters unknown */) {}
/* 001fb720 00000074 */ Utf8StrLen(/* parameters unknown */) {}
/* 001debd0 00000034 */ NetConnIdleShutdown(/* parameters unknown */) {}
/* 00239c64 000000ac */ memcpy(/* parameters unknown */) {}
/* 001f2b18 0000009c */ TourneyApiCreate(/* parameters unknown */) {}
/* 001b17d8 0000001c */ SHAPE_setmemcallbacks(/* parameters unknown */) {}
/* 001b0510 0000001c */ FONT_drawtextw(/* parameters unknown */) {}
/* 001fb5e8 0000001c */ Utf8Strip(/* parameters unknown */) {}
/* 002580a0 00000024 */ EAGL::RenderContext::operator new(/* parameters unknown */) {}
/* 00255fc8 00000044 */ AptSound::operator delete(/* parameters unknown */) {}
/* 001f6b28 000000f8 */ ProtoMangleCreate(/* parameters unknown */) {}
/* 0023ab98 00000020 */ _cleanup_r(/* parameters unknown */) {}
/* 0025b948 00000008 */ EASO::NetLobby::GetTKey(/* parameters unknown */) {}
/* 001f8000 0000010c */ WebOfferSetup(/* parameters unknown */) {}
/* 001f26c0 0000001c */ HLBApiGetTitleName(/* parameters unknown */) {}
/* 0025b078 00000008 */ EASO::NetVOIP::GetVoipRef(/* parameters unknown */) {}
/* 002523d0 00000008 */ AptKey::ContainsNativeHashVirtual(/* parameters unknown */) {}
/* 002564b8 00000008 */ AptXml::operator new [](/* parameters unknown */) {}
/* 0025b918 00000008 */ EASO::NetLobby::GetTickerServer(/* parameters unknown */) {}
/* 001ed0a8 00000028 */ TagFieldGetBulk(/* parameters unknown */) {}
/* 00256470 00000044 */ AptXml::operator delete(/* parameters unknown */) {}
/* 001f7c58 000000d4 */ _WebOfferGoto(/* parameters unknown */) {}
/* 001e5658 00000020 */ DispListSort(/* parameters unknown */) {}
/* 001b0590 0000001c */ FONT_getrecta(/* parameters unknown */) {}
/* 0025b330 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, unsigned int>(/* parameters unknown */) {}
/* 001de8e0 00000158 */ NetConnMAC(/* parameters unknown */) {}
/* 001de2f8 0000001c */ RpcBuffer(/* parameters unknown */) {}
/* 002529d8 000000a8 */ AptNativeHash::SetPrototypeGC(/* parameters unknown */) {}
/* 00252598 00000008 */ AptMouse::Release(/* parameters unknown */) {}
/* 001f3630 000000fc */ TourneyApiReqQuickJoin(/* parameters unknown */) {}
/* 00257ce0 00000024 */ EAGLInternal::VUmuAlloc::operator delete(/* parameters unknown */) {}
/* 001f0768 00000034 */ HLBApiSetDebugFunction(/* parameters unknown */) {}
/* 00202f30 00000000 */ DirtyDnasSimulateError(/* parameters unknown */) {}
/* 001e3890 000000c4 */ LobbyApiListFindByName(/* parameters unknown */) {}
/* 001eb600 000001c0 */ TagFieldSetString(/* parameters unknown */) {}
/* 00254f38 0000001c */ AptGlobal::Set(/* parameters unknown */) {}
/* 001f9f30 00000178 */ CryptSSC2StringEncrypt(/* parameters unknown */) {}
/* 001e35f8 000001d0 */ LobbyApiListAdd(/* parameters unknown */) {}
/* 00257000 00000024 */ AptValue::isMath(/* parameters unknown */) {}
/* 001f91b8 00000354 */ WebOfferGetCredit(/* parameters unknown */) {}
/* 001f9070 00000148 */ WebOfferGetAlert(/* parameters unknown */) {}
/* 001deb08 00000060 */ NetConnIdleProcs(/* parameters unknown */) {}
/* 001e5678 00000020 */ DispListFilt(/* parameters unknown */) {}
/* 001fdc28 000001ec */ BuddyApiRoster(/* parameters unknown */) {}
/* 0025bb18 00000008 */ EASO::NetLobby::SetXBOXDelay(/* parameters unknown */) {}
/* 001f5d18 00000008 */ ProtoAriesUpdate(/* parameters unknown */) {}
/* 001fe7f0 0000002c */ BuddyApiPresSame(/* parameters unknown */) {}
/* 0025b518 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *(*)>(/* parameters unknown */) {}
/* 00254960 00000078 */ AptDate::~AptDate(/* parameters unknown */) {}
/* 001edfd8 0000006c */ LobbyFindUserCreate(/* parameters unknown */) {}
/* 00252530 0000005c */ AptMouse::AptMouse(/* parameters unknown */) {}
/* 001f0ce0 00000028 */ HLBApiRegisterBuddyChangeCallback(/* parameters unknown */) {}
/* 0025bac0 00000008 */ EASO::NetLobby::GetMangleConnTimeout(/* parameters unknown */) {}
/* 00256c70 00000010 */ AptValue::IsReleaseAtEnd(/* parameters unknown */) {}
/* 001ed9f0 0000005c */ LobbySettingSetStringWithIndex(/* parameters unknown */) {}
/* 00257658 00000024 */ EASO::EasoHub::~EasoHub(/* parameters unknown */) {}
/* 001fb608 00000114 */ Utf8Replace(/* parameters unknown */) {}
/* 0023c610 00000050 */ malloc(/* parameters unknown */) {}
/* 0023b150 0000004c */ _printf_r(/* parameters unknown */) {}
/* 002528b8 00000074 */ EAStringC::operator=(/* parameters unknown */) {}
/* 00257a50 00000030 */ EASO::MySharedLoginAllocator::~MySharedLoginAllocator(/* parameters unknown */) {}
/* 001f2728 00000218 */ _TourneyApiDump(/* parameters unknown */) {}
/* 00258230 00000024 */ EAGL::TextureRenderContext::operator new [](/* parameters unknown */) {}
/* 001f5028 00000020 */ TourneyApiGetGameListCountByUser(/* parameters unknown */) {}
/* 00253ea8 00000008 */ AptCIH::getMorphInst(/* parameters unknown */) {}
/* 001eb3d8 00000078 */ TagFieldGetAddress(/* parameters unknown */) {}
/* 001ec7b8 000000e8 */ TagFieldSetDate(/* parameters unknown */) {}
/* 001f1ad0 000000e8 */ HLBListGameInviteBuddy(/* parameters unknown */) {}
/* 0023e390 00000024 */ strtodf(/* parameters unknown */) {}
/* 0023c3a8 0000006c */ vsnprintf(/* parameters unknown */) {}
/* 0025b060 00000008 */ EASO::NetVOIP::SetCurrentHardwareFlags(/* parameters unknown */) {}
/* 002540d8 00000048 */ AptCIH::isMorphInst(/* parameters unknown */) {}
/* 0023e930 00000078 */ strrev(/* parameters unknown */) {}
/* 00252130 0000005c */ AptMathObj::AptMathObj(/* parameters unknown */) {}
/* 001f54c8 0000003c */ TourneyApiFlagGet(/* parameters unknown */) {}
/* 001f5328 00000114 */ TourneyInitEncodeStrings(/* parameters unknown */) {}
/* 0025bab0 00000008 */ EASO::NetLobby::GetMangleTimeout(/* parameters unknown */) {}
/* 001f1198 000000a4 */ HLBMsgListGetFirstUnreadMsg(/* parameters unknown */) {}
/* 001e5938 00000080 */ HasherClear(/* parameters unknown */) {}
/* 00254a68 00000008 */ AptFloat::GetGCPointer(/* parameters unknown */) {}
/* 001b03d8 0000003c */ FONT_create(/* parameters unknown */) {}
/* 00255e48 00000024 */ AptInteger::operator new(/* parameters unknown */) {}
/* 001b0458 00000028 */ FONT_installdriverextensions(/* parameters unknown */) {}
/* 00242498 00000020 */ _toupper(/* parameters unknown */) {}
/* 0023e5c8 00000034 */ strtoul(/* parameters unknown */) {}
/* 001f19f8 000000d4 */ HLBListCancelGameInvite(/* parameters unknown */) {}
/* 0023e9a8 0000009c */ itoa(/* parameters unknown */) {}
/* 00242688 0000000c */ _localeconv_r(/* parameters unknown */) {}
/* 001fd7b8 000002c0 */ BuddyApiConnect(/* parameters unknown */) {}
/* 00256f38 00000024 */ AptValue::isFrameStack(/* parameters unknown */) {}
/* 0025b058 00000008 */ EASO::NetVOIP::SetNextDisconnectSilent(/* parameters unknown */) {}
/* 001e9c40 00000020 */ LobbyStatbookViewInfoStatus(/* parameters unknown */) {}
/* 00254cc0 00000024 */ AptFloat::operator delete(/* parameters unknown */) {}
/* 001e9a38 00000038 */ LobbyStatbookStatus(/* parameters unknown */) {}
/* 0025bb30 00000008 */ EASO::NetLobby::GetScoreReportScore0(/* parameters unknown */) {}
/* 00253d48 00000024 */ AptCIH::setHasClass(/* parameters unknown */) {}
/* 002437e0 0000005c */ _sbrk_r(/* parameters unknown */) {}
/* 00252b20 00000008 */ AptString::GetInternalString(/* parameters unknown */) {}
/* 00259b60 000001e4 */ void EASO::AddPair<int>(/* parameters unknown */) {}
/* 001b0f10 00000040 */ SHAPE_hotspots(/* parameters unknown */) {}
/* 001f6e68 00000094 */ ProtoMangleReport(/* parameters unknown */) {}
/* 001b0ac8 0000029c */ SHAPE_createat(/* parameters unknown */) {}
/* 00256d20 00000008 */ AptValue::c_movieClip(/* parameters unknown */) {}
/* 00200ee0 00000110 */ CryptMD5Update(/* parameters unknown */) {}
/* 001f0e88 00000020 */ HLBBudIsRealBuddy(/* parameters unknown */) {}
/* 0023b088 000000c4 */ __smakebuf(/* parameters unknown */) {}
/* 00254d38 0000005c */ AptGlobal::AptGlobal(/* parameters unknown */) {}
/* 00254d98 00000008 */ AptGlobal::AddRef(/* parameters unknown */) {}
/* 001ea908 00000128 */ TagFieldFind(/* parameters unknown */) {}
/* 001f1cb8 00000070 */ HLBListSort(/* parameters unknown */) {}
/* 00243780 00000060 */ _read_r(/* parameters unknown */) {}
/* 00256c40 00000014 */ AptValue::SetReleaseAtEnd(/* parameters unknown */) {}
/* 00255e98 00000024 */ AptInteger::operator new [](/* parameters unknown */) {}
/* 00258258 00000024 */ EAGL::TextureRenderContext::operator new [](/* parameters unknown */) {}
/* 0025adb8 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>(/* parameters unknown */) {}
/* 001fda78 000000dc */ _BuddyApiReconnect(/* parameters unknown */) {}
/* 001b0760 00000050 */ FONT_bsearch(/* parameters unknown */) {}
/* 001ef8c8 00000068 */ _HLBMsgListJettisonOldMsg(/* parameters unknown */) {}
/* 001e5450 0000008c */ DispListSet(/* parameters unknown */) {}
/* 002527c8 00000044 */ AptLoadVars::operator delete(/* parameters unknown */) {}
/* 00251ee8 00000024 */ AptObject::setInMainInst(/* parameters unknown */) {}
/* 0025a288 000002a8 */ EASO::UrlEncode(/* parameters unknown */) {}
/* 00256d78 00000014 */ AptValue::isXmlAttributes(/* parameters unknown */) {}
/* 002521e0 0000001c */ AptMathObj::GetNumGCPointers(/* parameters unknown */) {}
/* 001e3f98 00000574 */ LobbyApiExtractPlayRecord(/* parameters unknown */) {}
/* 001e56b8 0000001c */ DispListDirty(/* parameters unknown */) {}
/* 002551a0 0000004c */ AptNativeHash::FirstAllocation(/* parameters unknown */) {}
/* 00253358 00000008 */ AptFile::GetMainCharacter(/* parameters unknown */) {}
/* 002438a0 0000010c */ fflush(/* parameters unknown */) {}
/* 001ddf28 000001b4 */ RpcDone(/* parameters unknown */) {}
/* 00252660 00000044 */ AptMouse::operator delete(/* parameters unknown */) {}
/* 001f1c70 0000003c */ HLBListDeleteTempBuddy(/* parameters unknown */) {}
/* 0025bb40 00000008 */ EASO::EasoUtil::GetHomeUser(/* parameters unknown */) {}
/* 002570e0 00000024 */ AptValue::isDate(/* parameters unknown */) {}
/* 00252f48 00000010 */ AptNativeHash::ConvertAptValue(/* parameters unknown */) {}
/* 00257a28 00000028 */ EASO::SharedLoginThreadInterface::SetPriority(/* parameters unknown */) {}
/* 00256fd8 00000024 */ AptValue::isMouse(/* parameters unknown */) {}
/* 001e52d0 00000058 */ DispListDestroy(/* parameters unknown */) {}
/* 00257b68 00000048 */ EASO::MyMutex::Release(/* parameters unknown */) {}
/* 002545d8 00000008 */ AptTextFormat::operator new [](/* parameters unknown */) {}
/* 00252198 00000008 */ AptMathObj::Release(/* parameters unknown */) {}
/* 001f64e8 00000184 */ ProtoHttpRecv(/* parameters unknown */) {}
/* 001fa288 000000ec */ CheatCalcResponse(/* parameters unknown */) {}
/* 001ed798 00000068 */ LobbySettingCreate(/* parameters unknown */) {}
/* 0025b9d0 0000001c */ EASO::NetLobby::SetNewRoomPrefix(/* parameters unknown */) {}
/* 00256320 00000024 */ AptBoolean::operator new [](/* parameters unknown */) {}
/* 00252818 00000008 */ AptLoadVars::operator delete [](/* parameters unknown */) {}
/* 0025ade8 000001e4 */ void EASO::AddPair<char *>(/* parameters unknown */) {}
/* 00252ca0 00000024 */ AptString::operator new [](/* parameters unknown */) {}
/* 00251f60 0000001c */ AptObject::DestroyGCPointers(/* parameters unknown */) {}
/* 00257090 00000024 */ AptValue::isObject(/* parameters unknown */) {}
/* 001ee048 0000001c */ LobbyFindUserSetParams(/* parameters unknown */) {}
/* 00256968 00000078 */ AptGlobalExtensionObject::~AptGlobalExtensionObject(/* parameters unknown */) {}
/* 00255988 0000019c */ AptLinker::~AptLinker(/* parameters unknown */) {}
/* 002526b8 00000078 */ AptMouse::~AptMouse(/* parameters unknown */) {}
/* 00257fd8 00000028 */ EAGLInternal::GifState::SetAlphaBlend(/* parameters unknown */) {}
/* 0025ab40 00000114 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Swap(/* parameters unknown */) {}
/* 00256b30 00000010 */ AptValue::getGCRoot(/* parameters unknown */) {}
/* 00256cf0 00000008 */ AptValue::c_math(/* parameters unknown */) {}
/* 00252b40 00000008 */ AptString::GetGCPointer(/* parameters unknown */) {}
/* 00256e98 00000024 */ AptValue::isFloat(/* parameters unknown */) {}
/* 001ef4c8 0000015c */ _canAddBuddy(/* parameters unknown */) {}
/* 00256770 00000008 */ AptNone::GetNumGCPointers(/* parameters unknown */) {}
/* 00255f78 00000008 */ AptScriptFunction::operator new [](/* parameters unknown */) {}
/* 00244368 000000c0 */ _lo0bits(/* parameters unknown */) {}
/* 00256430 0000003c */ AptXml::operator new(/* parameters unknown */) {}
/* 00256ad0 00000030 */ AptValue::~AptValue(/* parameters unknown */) {}
/* 0025bc58 00000008 */ {anonymous}::CFESharedLoginAllocator::AddRef(/* parameters unknown */) {}
/* 00252ee8 0000005c */ AptLookup::~AptLookup(/* parameters unknown */) {}
/* 001eff50 00000144 */ HLBListCancelAllInvites(/* parameters unknown */) {}
/* 00254e08 0000001c */ AptGlobal::GetGCPointer(/* parameters unknown */) {}
/* 002581e0 00000024 */ EAGL::TextureRenderContext::operator new(/* parameters unknown */) {}
/* 0023c418 00000010 */ abort(/* parameters unknown */) {}
/* 0025ba98 00000008 */ EASO::NetLobby::SetManglerServerPort(/* parameters unknown */) {}
/* 001fcae8 00000098 */ BuddyApiDisconnect(/* parameters unknown */) {}
/* 00259e18 000001e4 */ void EASO::AddPair<char *>(/* parameters unknown */) {}
/* 00254188 00000048 */ AptCIH::isLevelInst(/* parameters unknown */) {}
/* 002448c0 00000068 */ __mcmp(/* parameters unknown */) {}
/* 00256cb8 00000008 */ AptValue::c_nativefunction(/* parameters unknown */) {}
/* 001f89c8 0000011c */ WebOfferHttpComplete(/* parameters unknown */) {}
/* 002545e8 0000009c */ AptTextFormat::~AptTextFormat(/* parameters unknown */) {}
/* 001ed350 000000e8 */ TagFieldFirst(/* parameters unknown */) {}
/* 001f99f0 00000118 */ ds_stristr(/* parameters unknown */) {}
/* 0025ba40 00000008 */ EASO::NetLobby::GetFaqServer(/* parameters unknown */) {}
/* 001e6ab0 00000084 */ PingManagerCancelRequest(/* parameters unknown */) {}
/* 00254000 00000048 */ AptCIH::isShapeInst(/* parameters unknown */) {}
/* 00253ea0 00000008 */ AptCIH::getStaticTextInst(/* parameters unknown */) {}
/* 001e8b28 00000054 */ LobbyRegGetLastResponse(/* parameters unknown */) {}
/* 001eb568 00000094 */ TagFieldGetToken(/* parameters unknown */) {}
/* 00256d60 00000014 */ AptValue::isXml(/* parameters unknown */) {}
/* 00253db8 000000d0 */ AptCIH::AptCIH(/* parameters unknown */) {}
/* 001e3958 00000050 */ LobbyApiListFindByKey(/* parameters unknown */) {}
/* 0025bae0 00000008 */ EASO::NetLobby::GetLobbyUpdateFreq(/* parameters unknown */) {}
/* 00258ef8 00000404 */ FONT_getrectx__H1ZUs_P4FONTPCX01PfN32_v(/* parameters unknown */) {}
/* 001f9e90 000000a0 */ CryptSSC2Apply(/* parameters unknown */) {}
/* 001fe5e8 00000168 */ BuddyApiSetForwarding(/* parameters unknown */) {}
/* 00256c58 00000018 */ AptValue::ClearReleaseAtEnd(/* parameters unknown */) {}
/* 00252820 00000028 */ EAStringC::EAStringC(/* parameters unknown */) {}
/* 00256df8 00000024 */ AptValue::isString(/* parameters unknown */) {}
/* 00242440 00000034 */ strtol(/* parameters unknown */) {}
/* 00253e90 00000008 */ AptCIH::getSpriteInstBase(/* parameters unknown */) {}
/* 001f44c8 000000e8 */ TourneyApiGetIdent(/* parameters unknown */) {}
/* 001f0518 00000134 */ _HLBApiInit(/* parameters unknown */) {}
/* 001ed9c8 00000028 */ LobbySettingSetString(/* parameters unknown */) {}
/* 00252390 00000008 */ AptKey::AddRef(/* parameters unknown */) {}
/* 00259300 000001fc */ void EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Insert<char *>(/* parameters unknown */) {}
/* 0025b910 00000008 */ EASO::NetLobby::GetPersona(/* parameters unknown */) {}
/* 00257bb0 0000001c */ EASO::MyMutex::Lock(/* parameters unknown */) {}
/* 00252b00 0000001c */ AptString::cat(/* parameters unknown */) {}
/* 001e8aa0 00000084 */ LobbyRegValidateAlert(/* parameters unknown */) {}
/* 00253048 00000078 */ AptMovieClip::~AptMovieClip(/* parameters unknown */) {}
/* 001b0448 0000000c */ FONT_installdriver(/* parameters unknown */) {}
/* 00255c18 00000008 */ AptInteger::GetGCPointer(/* parameters unknown */) {}
/* 001f8270 000001cc */ WebOfferCommand(/* parameters unknown */) {}
/* 002532b8 00000024 */ AptCharacterAnimationInst::operator new(/* parameters unknown */) {}
/* 00254ac8 0000009c */ AptFloat::AptFloat(/* parameters unknown */) {}
/* 001f14d8 0000005c */ HLBListGetBuddyByName(/* parameters unknown */) {}
/* 001e54f8 000000ac */ DispListDelByIndex(/* parameters unknown */) {}
/* 00252730 00000058 */ AptLoadVars::AptLoadVars(/* parameters unknown */) {}
/* 001f6308 000000c8 */ ProtoHttpCreate(/* parameters unknown */) {}
/* 00258048 00000024 */ EAGLInternal::DMAPerf::operator delete(/* parameters unknown */) {}
/* 00240220 000001b0 */ add_separators(/* parameters unknown */) {}
/* 00251fc8 0000001c */ AptObject::setProto(/* parameters unknown */) {}
/* 00253ec0 00000008 */ AptCIH::getShapeInst(/* parameters unknown */) {}
/* 001f0e68 0000001c */ HLBBudTempBuddyIs(/* parameters unknown */) {}
/* 001f7630 00000230 */ _WebOfferFind(/* parameters unknown */) {}
/* 001f0fd8 00000014 */ HLBBudIsJoinable(/* parameters unknown */) {}
/* 00256d38 00000008 */ AptValue::c_xmlattributes(/* parameters unknown */) {}
/* 002549d8 00000008 */ AptCharacterInst::PreDestroy(/* parameters unknown */) {}
/* 001fe750 00000060 */ BuddyApiGetForwarding(/* parameters unknown */) {}
/* 00254310 00000008 */ AptCIH::operator delete [](/* parameters unknown */) {}
/* 00253eb0 00000008 */ AptCIH::getButtonInst(/* parameters unknown */) {}
/* 002526b0 00000008 */ AptMouse::operator delete [](/* parameters unknown */) {}
/* 00251f10 0000000c */ AptObject::getInMainInst(/* parameters unknown */) {}
/* 002549e0 0000007c */ AptCharacterInst::~AptCharacterInst(/* parameters unknown */) {}
/* 00258af0 00000404 */ FONT_getrectx__H1ZUc_P4FONTPCX01PfN32_v(/* parameters unknown */) {}
/* 002580f0 00000024 */ EAGL::RenderContext::operator new [](/* parameters unknown */) {}
/* 00259b30 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>(/* parameters unknown */) {}
/* 0025b998 00000008 */ EASO::NetLobby::SetLastLogin(/* parameters unknown */) {}
/* 00256918 00000008 */ AptGlobalExtensionObject::ContainsNativeHashVirtual(/* parameters unknown */) {}
/* 0025bbc0 00000030 */ {anonymous}::CFESharedLoginAllocator::~CFESharedLoginAllocator(/* parameters unknown */) {}
/* 002566b8 00000008 */ AptValue::objectMemberLookup(/* parameters unknown */) {}
/* 001eb7c0 000001f4 */ TagFieldGetString(/* parameters unknown */) {}
/* 00254b68 00000110 */ AptFloat::Create(/* parameters unknown */) {}
/* 001f5960 0000008c */ ProtoAriesUnconnect(/* parameters unknown */) {}
/* 00257630 00000010 */ EAGL::TARExtension::GetWidth(/* parameters unknown */) {}
/* 001f1000 00000058 */ HLBBudCanVoiceChat(/* parameters unknown */) {}
/* 001e55a8 0000005c */ DispListDel(/* parameters unknown */) {}
/* 001e4830 000000b0 */ LobbyApiExtractUserAddr(/* parameters unknown */) {}
/* 00256c30 00000010 */ AptValue::decGCRoot(/* parameters unknown */) {}
/* 00252fb0 0000001c */ AptPrototype::GetGCPointer(/* parameters unknown */) {}
/* 001ef328 0000009c */ _removeFirstTempBuddy(/* parameters unknown */) {}
/* 001e0a40 000000a4 */ LobbyApiCancelCB(/* parameters unknown */) {}
/* 001fe7e0 00000010 */ BuddyApiPresInit(/* parameters unknown */) {}
/* 0023e360 0000002c */ strtod(/* parameters unknown */) {}
/* 00258718 000003d4 */ void FONT_drawtextx<unsigned short>(/* parameters unknown */) {}
/* 00254e68 00000044 */ AptGlobal::operator delete(/* parameters unknown */) {}
/* 00252f58 0000001c */ AptPrototype::DestroyGCPointers(/* parameters unknown */) {}
/* 001deb68 00000068 */ NetConnIdle(/* parameters unknown */) {}
/* 00254120 00000064 */ AptCIH::isAnimationInst(/* parameters unknown */) {}
/* 00254a60 00000008 */ AptFloat::GetNumGCPointers(/* parameters unknown */) {}
/* 001e86a0 00000148 */ LobbyRegSetContextCB(/* parameters unknown */) {}
/* 0025b2c0 0000006c */ void EASO::AddPair<LobbyRegContextE>(/* parameters unknown */) {}
/* 001fa0a8 00000118 */ CryptSSC2StringDecrypt(/* parameters unknown */) {}
/* 002561a0 00000110 */ AptBoolean::Create(/* parameters unknown */) {}
/* 001f1630 00000020 */ HLBListGetBuddyCount(/* parameters unknown */) {}
/* 001b1338 00000204 */ SHAPE_getpixeladr(/* parameters unknown */) {}
/* 001dd738 000001a8 */ SockaddrInParse2(/* parameters unknown */) {}
/* 001eaa30 00000078 */ TagFieldFind2(/* parameters unknown */) {}
/* 002566c0 00000008 */ AptValue::objectMemberSet(/* parameters unknown */) {}
/* 001b0d68 00000100 */ SHAPE_create(/* parameters unknown */) {}
/* 002524b0 00000008 */ AptKey::operator delete [](/* parameters unknown */) {}
/* 001f0ff0 0000000c */ HLBBudGetVOIPState(/* parameters unknown */) {}
/* 002574d8 00000024 */ AptCharacterTextInst::operator delete(/* parameters unknown */) {}
/* 0023c4f0 0000001c */ atof(/* parameters unknown */) {}
/* 001f85b0 00000418 */ WebOfferHttp(/* parameters unknown */) {}
/* 001f5798 00000080 */ ProtoAriesDestroy(/* parameters unknown */) {}
/* 001ee568 0000001c */ _listRemovePreConnTempBuddy(/* parameters unknown */) {}
/* 001e9b48 00000008 */ LobbyStatbookViewCount(/* parameters unknown */) {}
/* 0023e8c0 0000006c */ strround(/* parameters unknown */) {}
/* 0025b6d0 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >(/* parameters unknown */) {}
/* 001f16f0 00000024 */ HLBListUnBlockBuddy(/* parameters unknown */) {}
/* 002541d0 000000b0 */ AptCIH::isSpriteInstBase(/* parameters unknown */) {}
/* 001f1cb0 00000008 */ HLBListSetSortFunction(/* parameters unknown */) {}
/* 001e0638 00000044 */ LobbyApiSetCallback(/* parameters unknown */) {}
/* 001f3500 00000020 */ TourneyApiReqCancelUpdates(/* parameters unknown */) {}
/* 001e8b80 00000010 */ LobbyRegSetBehavior(/* parameters unknown */) {}
/* 00252f88 00000008 */ AptPrototype::IsGarbageCollected(/* parameters unknown */) {}
/* 001df370 000000a4 */ NetConnShutdown(/* parameters unknown */) {}
/* 002560a0 00000008 */ AptBoolean::GetGCPointer(/* parameters unknown */) {}
/* 00252ee0 00000008 */ AptLookup::GetGCPointer(/* parameters unknown */) {}
/* 001f8e98 00000114 */ WebOfferResource(/* parameters unknown */) {}
/* 00259b00 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, int>(/* parameters unknown */) {}
/* 00252330 0000005c */ AptKey::AptKey(/* parameters unknown */) {}
/* 001f9690 0000013c */ WebOfferGetPromo(/* parameters unknown */) {}
/* 00257c48 00000008 */ EASO::SharedLoginThreadInterface::IsActive(/* parameters unknown */) {}
/* 001f7f58 00000048 */ WebOfferCreate(/* parameters unknown */) {}
/* 00239ec4 00000130 */ strcat(/* parameters unknown */) {}
/* 001f2c40 000000c4 */ TourneyApiReqAuthent(/* parameters unknown */) {}
/* 002582b0 00000008 */ EAGL::TextureRenderContext::operator delete(/* parameters unknown */) {}
/* 001e3e08 0000018c */ LobbyApiExtractUserSet(/* parameters unknown */) {}
/* 00251e48 00000018 */ EAStringC::IsEmpty(/* parameters unknown */) {}
/* 0025ac58 0000012c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator EASO::String::DefaultPolicy::Copy<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::DbgIterator>(/* parameters unknown */) {}
/* 00256c90 00000008 */ AptValue::c_register(/* parameters unknown */) {}
/* 001ef628 0000010c */ _MsgListGetOldestMsgForBuddy(/* parameters unknown */) {}
/* 00251be8 00000078 */ AptNativeFunction::~AptNativeFunction(/* parameters unknown */) {}
/* 00256dc0 00000010 */ AptValue::isUndefined(/* parameters unknown */) {}
/* 00251e60 00000050 */ AptObject::AptObject(/* parameters unknown */) {}
/* 001fea80 00000d68 */ BuddyApiUpdate(/* parameters unknown */) {}
/* 001dd320 00000048 */ SockaddrCompare(/* parameters unknown */) {}
/* 00256cc8 00000008 */ AptValue::c_array(/* parameters unknown */) {}
/* 00256cd8 00000008 */ AptValue::c_key(/* parameters unknown */) {}
/* 00256b50 00000020 */ AptValue::setRefCount(/* parameters unknown */) {}
/* 001e5b78 000000c8 */ HashStrAdd(/* parameters unknown */) {}
/* 001f67d0 00000090 */ ProtoHttpSend(/* parameters unknown */) {}
/* 001fea50 0000000c */ BuddyApiRecv(/* parameters unknown */) {}
/* 0023c320 0000002c */ vprintf(/* parameters unknown */) {}
/* 001ee720 0000006c */ _HLBRosterDestroy(/* parameters unknown */) {}
/* 00256768 00000008 */ AptNone::Release(/* parameters unknown */) {}
/* 0025bb08 00000008 */ EASO::NetLobby::SetKeyPressTimer(/* parameters unknown */) {}
/* 001e5ed8 00000064 */ HashNumReplace(/* parameters unknown */) {}
/* 001f1bf8 00000074 */ HLBListUnFlagTempBuddy(/* parameters unknown */) {}
/* 002562f8 00000024 */ AptBoolean::operator delete(/* parameters unknown */) {}
/* 001fea60 0000000c */ BuddyApiRegisterBuddyChangeCallback(/* parameters unknown */) {}
/* 00254308 00000008 */ AptCIH::operator new [](/* parameters unknown */) {}
/* 001ea2a0 00000148 */ TagFieldFormat(/* parameters unknown */) {}
/* 002581a0 00000008 */ EAGL::RenderContext::GetDepthBufferType(/* parameters unknown */) {}
/* 001f0d08 00000028 */ HLBApiRegisterGameInviteCallback(/* parameters unknown */) {}
/* 001f0fc0 00000014 */ HLBBudIsNoReplyBud(/* parameters unknown */) {}
/* 001f0958 00000014 */ HLBApiGetConnectState(/* parameters unknown */) {}
/* 001f5638 0000015c */ ProtoAriesCreate(/* parameters unknown */) {}
/* 001df470 00000168 */ NetConnInstallResetHandler(/* parameters unknown */) {}
/* 00256cd0 00000008 */ AptValue::c_sound(/* parameters unknown */) {}
/* 001ee588 00000060 */ _listMsgItemDestroy(/* parameters unknown */) {}
/* 0025b048 00000008 */ EASO::NetVOIP::GetOutputSetting(/* parameters unknown */) {}
/* 0023abe0 0000040c */ fseek(/* parameters unknown */) {}
/* 001e0e20 00000048 */ LobbyApiControl(/* parameters unknown */) {}
/* 00244668 00000100 */ _pow5mult(/* parameters unknown */) {}
/* 001e11b0 00000760 */ LobbyApiUpdate(/* parameters unknown */) {}
/* 001f0cb8 00000028 */ HLBApiRegisterNewMsgCallback(/* parameters unknown */) {}
/* 001e0680 00000044 */ LobbyApiClearCallback(/* parameters unknown */) {}
/* 001e9798 00000008 */ LobbyStatbookCancel(/* parameters unknown */) {}
/* 00251ed8 0000000c */ AptObject::getHasClass(/* parameters unknown */) {}
/* 00253330 00000024 */ AptCharacterAnimationInst::operator delete [](/* parameters unknown */) {}
/* 00257e20 00000018 */ EAGLInternal::GifState::SetPrim(/* parameters unknown */) {}
/* 00242478 00000020 */ _tolower(/* parameters unknown */) {}
/* 001dd588 00000018 */ SocketNtohs(/* parameters unknown */) {}
/* 001f0eb8 0000001c */ HLBBudGetState(/* parameters unknown */) {}
/* 001e3c80 00000068 */ LobbyApiListObjectUpdate(/* parameters unknown */) {}
/* 00254d10 00000024 */ AptFloat::operator delete [](/* parameters unknown */) {}
/* 002569e0 0000007c */ AptValue::AptValue(/* parameters unknown */) {}
/* 00256b90 00000028 */ AptValue::setIsDefined(/* parameters unknown */) {}
/* 001f1d48 00000084 */ HLBListBuddyWithMsg(/* parameters unknown */) {}
/* 001b05b0 0000001c */ FONT_getrectw(/* parameters unknown */) {}
/* 00257960 0000006c */ EASO::SharedLoginThreadInterface::Begin(/* parameters unknown */) {}
/* 001ee548 0000001c */ _listItemGetPreConnTempBuddyByIndex(/* parameters unknown */) {}
/* 0025b390 000000e8 */ void EASO::AddPair<unsigned int>(/* parameters unknown */) {}
/* 002566c8 00000034 */ AptValue::DeleteThis(/* parameters unknown */) {}
/* 001ee5e8 00000044 */ _rosterCount(/* parameters unknown */) {}
/* 001ef188 0000019c */ _HLBListRemoveBuddy(/* parameters unknown */) {}
/* 00252018 00000008 */ AptObject::ContainsNativeHashVirtual(/* parameters unknown */) {}
/* 001e1010 00000008 */ LobbyApiCancelPingCallback(/* parameters unknown */) {}
/* 00255c10 00000008 */ AptInteger::GetNumGCPointers(/* parameters unknown */) {}
/* 001de318 000000d0 */ RpcPeek(/* parameters unknown */) {}
/* 001e7b40 000000ac */ LobbyRegCreate(/* parameters unknown */) {}
/* 002426f0 000000ec */ memchr(/* parameters unknown */) {}
/* 0023b418 00000078 */ _sprintf_r(/* parameters unknown */) {}
/* 00241da8 000002f4 */ _free_r(/* parameters unknown */) {}
/* 001f7fa0 00000050 */ WebOfferDestroy(/* parameters unknown */) {}
/* 00256928 0000001c */ AptGlobalExtensionObject::GetNumGCPointers(/* parameters unknown */) {}
/* 00252220 0000003c */ AptMathObj::operator new(/* parameters unknown */) {}
/* 001f0110 0000006c */ _HLBEnableSorting(/* parameters unknown */) {}
/* 00257500 0000003c */ AptXmlNode::operator new(/* parameters unknown */) {}
/* 00254ce8 00000024 */ AptFloat::operator new [](/* parameters unknown */) {}
/* 00255ec0 00000024 */ AptInteger::operator delete [](/* parameters unknown */) {}
/* 0025baf8 00000008 */ EASO::NetLobby::SetRoomUpdateFreq(/* parameters unknown */) {}
/* 001ec120 00000318 */ TagFieldGetStructure(/* parameters unknown */) {}
/* 001f0f08 00000028 */ HLBBudIsSameProduct(/* parameters unknown */) {}
/* 001ebb90 000001f0 */ TagFieldGetBinary(/* parameters unknown */) {}
/* 00256b20 00000010 */ AptValue::getGCMark(/* parameters unknown */) {}
/* 0025bb10 00000008 */ EASO::NetLobby::GetXBOXDelay(/* parameters unknown */) {}
/* 001dd600 00000058 */ SocketInAddrGetText(/* parameters unknown */) {}
/* 00251cb0 000000b4 */ AptArray::SetAt(/* parameters unknown */) {}
/* 001f21f8 000000a8 */ HLBApiPresenceSend(/* parameters unknown */) {}
/* 001ffb68 00000008 */ BuddyApiGetTitleName(/* parameters unknown */) {}
/* 001f6f08 00000080 */ ProtoMangleControl(/* parameters unknown */) {}
/* 001eea50 00000200 */ _HLBConnectCallback(/* parameters unknown */) {}
/* 00254da0 00000008 */ AptGlobal::Release(/* parameters unknown */) {}
/* 001fc4c0 00000008 */ _BuddyApiDoXboxAction(/* parameters unknown */) {}
/* 002524a8 00000008 */ AptKey::operator new [](/* parameters unknown */) {}
/* 001e5328 00000068 */ DispListClear(/* parameters unknown */) {}
/* 0023e700 00000074 */ strstr(/* parameters unknown */) {}
/* 00257c50 00000068 */ EAGLInternal::DMAAccumulator::Close(/* parameters unknown */) {}
/* 001b1008 0000005c */ SHAPE_metal(/* parameters unknown */) {}
/* 00256010 00000008 */ AptSound::operator new [](/* parameters unknown */) {}
/* 002566b0 00000008 */ AptValue::ContainsNativeHashVirtual(/* parameters unknown */) {}
/* 00257c40 00000008 */ EASO::SharedLoginThreadInterface::GetEntryFunc(/* parameters unknown */) {}
/* 0025bae8 00000008 */ EASO::NetLobby::SetLobbyUpdateFreq(/* parameters unknown */) {}
/* 0025bc60 00000008 */ {anonymous}::CFESharedLoginAllocator::Release(/* parameters unknown */) {}
/* 001f4a60 0000010c */ TourneyApiGetUserByIndex(/* parameters unknown */) {}
/* 001f20c0 0000001c */ HLBApiPresenceDiff(/* parameters unknown */) {}
/* 0025b178 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, LobbyRegContextE>(/* parameters unknown */) {}
/* 001f5588 000000b0 */ TourneyApiFlagBuild(/* parameters unknown */) {}
/* 001e34c0 00000040 */ LobbyApiListCallback(/* parameters unknown */) {}
/* 0025b990 00000008 */ EASO::NetLobby::GetLastLogin(/* parameters unknown */) {}
/* 00252f90 0000001c */ AptPrototype::GetNumGCPointers(/* parameters unknown */) {}
/* 001eec50 00000164 */ _HLBOpCallback(/* parameters unknown */) {}
/* 001f61b0 00000080 */ ProtoAriesSecure(/* parameters unknown */) {}
/* 001eb9b8 000000cc */ TagFieldSetBinary(/* parameters unknown */) {}
/* 001eda88 00000028 */ LobbySettingSetNumber(/* parameters unknown */) {}
/* 00257bf0 00000050 */ EASO::SharedLoginThreadInterface::EntryProc(/* parameters unknown */) {}
/* 00251da8 00000044 */ AptArray::operator delete(/* parameters unknown */) {}
/* 00254688 00000048 */ TextFormat::copyTextFormatObj(/* parameters unknown */) {}
/* 00252008 00000008 */ AptObject::getNativeHashVirtual(/* parameters unknown */) {}
/* 001f9510 0000017c */ WebOfferSetCredit(/* parameters unknown */) {}
/* 00255e70 00000024 */ AptInteger::operator delete(/* parameters unknown */) {}
/* 00243720 00000060 */ _open_r(/* parameters unknown */) {}
/* 00252590 00000008 */ AptMouse::AddRef(/* parameters unknown */) {}
/* 001e0e68 000001a4 */ LobbyApiPing(/* parameters unknown */) {}
/* 00254a70 00000054 */ AptFloat::~AptFloat(/* parameters unknown */) {}
/* 00251fe8 0000001c */ AptObject::setPrototype(/* parameters unknown */) {}
/* 0023a6f8 000001c8 */ strncmp(/* parameters unknown */) {}
/* 00259db8 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>(/* parameters unknown */) {}
/* 0025bc68 00000064 */ bool EASO::String::operator==<EASO::String::EncodingUtf8, EASO::FifaStringPolicy, EASO::FifaStringPolicy>(/* parameters unknown */) {}
/* 00257228 00000084 */ AptCharacterStaticTextInst::~AptCharacterStaticTextInst(/* parameters unknown */) {}
/* 002525a0 0000001c */ AptMouse::DestroyGCPointers(/* parameters unknown */) {}
/* 0025a140 00000148 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Reserve(/* parameters unknown */) {}
/* 001faf70 00000024 */ LobbyLocalizeDestroy(/* parameters unknown */) {}
/* 00251d68 0000003c */ AptArray::operator new(/* parameters unknown */) {}
/* 0025b1d8 000000e8 */ void EASO::AddPair<LobbyRegContextE>(/* parameters unknown */) {}
/* 00257870 00000098 */ EASO::SharedLoginThreadInterface::Instance(/* parameters unknown */) {}
/* 001f0d30 00000028 */ HLBApiRegisterBuddyPresenceCallback(/* parameters unknown */) {}
/* 001f6d88 00000008 */ ProtoMangleUpdate(/* parameters unknown */) {}
/* 001ee388 00000084 */ _verifyBuddyList(/* parameters unknown */) {}
/* 001f4e30 00000170 */ TourneyApiGetGameByUser(/* parameters unknown */) {}
/* 00254950 00000008 */ AptDate::operator new [](/* parameters unknown */) {}
/* 001e5a58 000000a4 */ HasherFlush(/* parameters unknown */) {}
/* 0023a8c0 000001c8 */ strncpy(/* parameters unknown */) {}
/* 001faf98 00000358 */ LobbyLocalizeData(/* parameters unknown */) {}
/* 001f45b0 000001a8 */ TourneyApiGetNews(/* parameters unknown */) {}
/* 001fd6d8 0000000c */ BuddyApiDebug(/* parameters unknown */) {}
/* 001ef860 00000064 */ _HLBMsgListDeleteOldestMsgForBud(/* parameters unknown */) {}
/* 0023e600 0000007c */ strcasecmp(/* parameters unknown */) {}
/* 002523e0 0000001c */ AptKey::GetNumGCPointers(/* parameters unknown */) {}
/* 00255e40 00000008 */ AptInteger::GetInt(/* parameters unknown */) {}
/* 002546d0 000001f4 */ AptDate::AptDate(/* parameters unknown */) {}
/* 00242e18 0000062c */ sn_log(/* parameters unknown */) {}
/* 001ea510 00000180 */ TagFieldRename(/* parameters unknown */) {}
/* 00252c50 00000024 */ AptString::operator new(/* parameters unknown */) {}
/* 001f4fa0 00000014 */ TourneyApiGetFetchTime(/* parameters unknown */) {}
/* 00256dd0 00000024 */ AptValue::isLookup(/* parameters unknown */) {}
/* 001deab8 0000004c */ NetConnIdleDel(/* parameters unknown */) {}
/* 001eb038 0000010c */ TagFieldGetNumber64(/* parameters unknown */) {}
/* 001b10d0 00000098 */ SHAPE_type(/* parameters unknown */) {}
/* 00256e20 00000024 */ AptValue::isBoolean(/* parameters unknown */) {}
/* 001f3380 00000080 */ TourneyApiReqMember(/* parameters unknown */) {}
/* 002568e0 00000008 */ AptGlobalExtensionObject::AddRef(/* parameters unknown */) {}
/* 0023e3b8 00000210 */ _strtoul_r(/* parameters unknown */) {}
/* 001b0ef8 00000018 */ SHAPE_depth(/* parameters unknown */) {}
/* 00256c98 00000008 */ AptValue::c_float(/* parameters unknown */) {}
/* 001f0ed8 00000014 */ HLBBudIsPassive(/* parameters unknown */) {}
/* 00253e98 00000008 */ AptCIH::getTextInst(/* parameters unknown */) {}
/* 001e96d8 000000bc */ LobbyStatbookCreate(/* parameters unknown */) {}
/* 001f0c38 00000060 */ HLBApiFindUsers(/* parameters unknown */) {}
/* 00256710 00000050 */ AptHash::~AptHash(/* parameters unknown */) {}
/* 00255f80 00000008 */ AptScriptFunction::operator delete [](/* parameters unknown */) {}
/* 001dee88 00000088 */ NetConnQuery(/* parameters unknown */) {}
/* 001b0f50 00000040 */ SHAPE_longname(/* parameters unknown */) {}
/* 0025b970 0000001c */ EASO::NetLobby::setTKey(/* parameters unknown */) {}
/* 00243608 00000058 */ _close_r(/* parameters unknown */) {}
/* 002544a8 000000a4 */ AptTextFormat::AptTextFormat(/* parameters unknown */) {}
/* 001f7ff0 00000010 */ WebOfferClear(/* parameters unknown */) {}
/* 001ee4e0 0000001c */ _listDestroy(/* parameters unknown */) {}
/* 0025b9a0 00000008 */ EASO::NetLobby::GetCategoryUrl(/* parameters unknown */) {}
/* 001e8b90 000003ec */ LobbyMSort(/* parameters unknown */) {}
/* 001f15a0 0000008c */ HLBListGetIndexByName(/* parameters unknown */) {}
/* 002548c8 0000003c */ AptDate::operator new(/* parameters unknown */) {}
/* 0023e778 00000024 */ strtok(/* parameters unknown */) {}
/* 002562b0 00000014 */ AptBoolean::Destroy(/* parameters unknown */) {}
/* 002429d0 00000444 */ sn_fmod(/* parameters unknown */) {}
/* 00254fb8 00000044 */ AptScriptColour::operator delete(/* parameters unknown */) {}
/* 001fe820 00000008 */ BuddyApiPresJoinable(/* parameters unknown */) {}
/* 001ec438 000000d8 */ TagFieldSetEpoch(/* parameters unknown */) {}
/* 00258198 00000008 */ EAGL::RenderContext::SetDepthBufferType(/* parameters unknown */) {}
/* 002551f0 00000554 */ AptNativeHash::HashSet(/* parameters unknown */) {}
/* 00239bd0 00000094 */ memcmp(/* parameters unknown */) {}
/* 001fc7e8 00000234 */ _BuddyApiHandlePGETMsg(/* parameters unknown */) {}
/* 002542c0 00000044 */ AptCIH::operator delete(/* parameters unknown */) {}
/* 002522b0 00000008 */ AptMathObj::operator delete [](/* parameters unknown */) {}
/* 002523d8 00000008 */ AptKey::IsGarbageCollected(/* parameters unknown */) {}
/* 00252600 0000001c */ AptMouse::GetGCPointer(/* parameters unknown */) {}
/* 00256fb0 00000024 */ AptValue::isKey(/* parameters unknown */) {}
/* 001f99a0 0000004c */ ds_snzprintf(/* parameters unknown */) {}
/* 0023b518 00000098 */ sscanf(/* parameters unknown */) {}
/* 002564c0 00000008 */ AptXml::operator delete [](/* parameters unknown */) {}
/* 001f2120 00000060 */ HLBApiPresenceJoinable(/* parameters unknown */) {}
/* 001f0f98 00000028 */ HLBBudGetPresenceExtra(/* parameters unknown */) {}
/* 001f0ad0 00000074 */ HLBApiSuspend(/* parameters unknown */) {}
/* 001b1540 00000094 */ GetSwizzledPixelOffset(/* parameters unknown */) {}
/* 001e39a8 00000068 */ LobbyApiListAvailable(/* parameters unknown */) {}
/* 00256870 00000008 */ AptExtern::GetNumGCPointers(/* parameters unknown */) {}
/* 00256c80 00000008 */ AptValue::c_lookup(/* parameters unknown */) {}
/* 00243a80 000000dc */ __sfp(/* parameters unknown */) {}
/* 0023a540 000001b4 */ strncat(/* parameters unknown */) {}
/* 0025bb00 00000008 */ EASO::NetLobby::GetKeyPressTimer(/* parameters unknown */) {}
/* 001eaf98 0000009c */ TagFieldGetNumber(/* parameters unknown */) {}
/* 001dd5a0 00000060 */ SocketNtohl(/* parameters unknown */) {}
/* 002582c8 00000074 */ EAGLInternal::VUmuAlloc::Flush(/* parameters unknown */) {}
/* 001fe878 00000008 */ BuddyApiPresExFlags(/* parameters unknown */) {}
/* 001f18e8 00000078 */ HLBListAnswerInvite(/* parameters unknown */) {}
/* 00254dc8 00000008 */ AptGlobal::getNativeHashVirtual(/* parameters unknown */) {}
/* 00256d90 00000014 */ AptValue::isLoadVars(/* parameters unknown */) {}
/* 0025bcf0 0000002c */ EACSL::SharedLoginData::Reset(/* parameters unknown */) {}
/* 00256638 00000064 */ AptValue::ForceDelete(/* parameters unknown */) {}
/* 0023b5b0 00000064 */ __sread(/* parameters unknown */) {}
/* 001de830 000000a0 */ RpcStopEE(/* parameters unknown */) {}
/* 001e0ae8 00000210 */ LobbyApiInfoInt(/* parameters unknown */) {}
/* 00259d48 0000006c */ void EASO::AddPair<int>(/* parameters unknown */) {}
/* 001e32d8 00000064 */ LobbyApiListFlush(/* parameters unknown */) {}
/* 001ed918 00000058 */ LobbySettingGetString(/* parameters unknown */) {}
/* 00243d10 00000114 */ fread(/* parameters unknown */) {}
/* 00252010 00000008 */ AptObject::getNativeHash(/* parameters unknown */) {}
/* 001df418 00000054 */ NetConnSleep(/* parameters unknown */) {}
/* 001fd3f8 0000017c */ _BuddyApiHandleSearchedUserMsg(/* parameters unknown */) {}
/* 00256d28 00000008 */ AptValue::c_xmlnode(/* parameters unknown */) {}
/* 001f0ea8 00000010 */ HLBBudIsInGroup(/* parameters unknown */) {}
/* 00256f88 00000024 */ AptValue::isSound(/* parameters unknown */) {}
/* 001f7998 00000108 */ _WebOfferActionClear(/* parameters unknown */) {}
/* 001f0e30 00000038 */ HLBBudGetTitle(/* parameters unknown */) {}
/* 0025ad88 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, char *>(/* parameters unknown */) {}
/* 001f1d28 0000001c */ HLBListChanged(/* parameters unknown */) {}
/* 001fd6c8 00000008 */ BuddyApiStatus(/* parameters unknown */) {}
/* 00256708 00000008 */ AptValue::DestroyGCPointers(/* parameters unknown */) {}
/* 00257e00 0000001c */ EAGLInternal::GifState::SetFST(/* parameters unknown */) {}
/* 0023c9e8 00000060 */ __malloc_lock(/* parameters unknown */) {}
/* 0025a588 000000bc */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString(/* parameters unknown */) {}
/* 001fe880 00000024 */ BuddyApiPresExtra(/* parameters unknown */) {}
/* 0025ba68 00000008 */ EASO::NetLobby::GetTosServer(/* parameters unknown */) {}
/* 001f22a0 00000060 */ HLBApiPresenceVOIPSend(/* parameters unknown */) {}
/* 001f8fb0 000000bc */ WebOfferGetBusy(/* parameters unknown */) {}
/* 001ffe10 00000060 */ ProtoNameSync(/* parameters unknown */) {}
/* 00200ea8 00000038 */ CryptMD5Init(/* parameters unknown */) {}
/* 001f0d58 00000008 */ HLBBudGetName(/* parameters unknown */) {}
/* 001e3298 00000040 */ LobbyApiListCreate(/* parameters unknown */) {}
/* 00244fc8 000000bc */ _calloc_r(/* parameters unknown */) {}
/* 0025b6a8 00000028 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCastImpl<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >(/* parameters unknown */) {}
/* 001e7bf0 00000040 */ LobbyRegDestroy(/* parameters unknown */) {}
/* 001ea3e8 00000128 */ TagFieldDelete(/* parameters unknown */) {}
/* 001fd1f0 00000204 */ _BuddyApiHandleRNOTMsg(/* parameters unknown */) {}
/* 00254908 00000044 */ AptDate::operator delete(/* parameters unknown */) {}
/* 00252aa0 00000040 */ AptString::cpy(/* parameters unknown */) {}
/* 001e8678 00000024 */ LobbyRegGetStatus(/* parameters unknown */) {}
/* 00258020 00000018 */ EAGL::Model::ModelShape_iterator::ModelShape_iterator(/* parameters unknown */) {}
/* 0023ab68 0000002c */ fopen(/* parameters unknown */) {}
/* 001f3048 000000fc */ TourneyApiReqJoin(/* parameters unknown */) {}
/* 001f6d90 000000d8 */ ProtoMangleComplete(/* parameters unknown */) {}
/* 00251df8 00000008 */ AptArray::operator delete [](/* parameters unknown */) {}
/* 00257e78 0000001c */ EAGLInternal::GifState::SetAlpha(/* parameters unknown */) {}
/* 00257ab0 0000001c */ EASO::MySharedLoginAllocator::Free(/* parameters unknown */) {}
/* 00239e0c 000000b8 */ memset(/* parameters unknown */) {}
/* 00256d00 00000008 */ AptValue::c_object(/* parameters unknown */) {}
/* 002526a8 00000008 */ AptMouse::operator new [](/* parameters unknown */) {}
/* 002545e0 00000008 */ AptTextFormat::operator delete [](/* parameters unknown */) {}
/* 001fea10 0000003c */ BuddyApiBroadcast(/* parameters unknown */) {}
/* 0025b630 0000006c */ void EASO::AddPair<char *(*)>(/* parameters unknown */) {}
/* 0025bb60 00000008 */ EASO::EasoUtil::GetTID(/* parameters unknown */) {}
/* 00257050 0000003c */ AptValue::isCIH(/* parameters unknown */) {}
/* 00252020 0000003c */ AptObject::operator new(/* parameters unknown */) {}
/* 001e6448 00000048 */ PingManagerDestroy(/* parameters unknown */) {}
/* 001f98a8 000000f8 */ WebOfferGetNews(/* parameters unknown */) {}
/* 001ec510 000002a8 */ TagFieldGetEpoch(/* parameters unknown */) {}
/* 001ed0d0 0000027c */ TagFieldGetDelim(/* parameters unknown */) {}
/* 001e69e0 00000070 */ PingManagerInvalidateAddress(/* parameters unknown */) {}
/* 00255010 00000090 */ AptCharacterButtonInst::~AptCharacterButtonInst(/* parameters unknown */) {}
/* 001f9bc0 0000003c */ ds_strnzcpy(/* parameters unknown */) {}
/* 002568e8 00000008 */ AptGlobalExtensionObject::Release(/* parameters unknown */) {}
/* 00252c00 00000050 */ AptString::Destroy(/* parameters unknown */) {}
/* 00255c20 00000054 */ AptInteger::~AptInteger(/* parameters unknown */) {}
/* 001b04f0 0000001c */ FONT_drawtexta(/* parameters unknown */) {}
/* 00244ab8 000000c8 */ _ulp(/* parameters unknown */) {}
/* 00257d58 00000088 */ EAGLInternal::PropertyParser::FindTokenEnd(/* parameters unknown */) {}
/* 001f6c20 00000064 */ ProtoMangleDestroy(/* parameters unknown */) {}
/* 001ee4a0 0000001c */ _listGetCount(/* parameters unknown */) {}
/* 0025bb20 00000008 */ EASO::NetLobby::GetScoreReportName0(/* parameters unknown */) {}
/* 0023b700 00000020 */ __sclose(/* parameters unknown */) {}
/* 0023ea48 0000058c */ fftoa(/* parameters unknown */) {}
/* 001e4f10 00000188 */ LobbyApiGetTrophies(/* parameters unknown */) {}
/* 00242210 0000022c */ _strtol_r(/* parameters unknown */) {}
/* 001ebd80 0000039c */ TagFieldSetStructure(/* parameters unknown */) {}
/* 00252f78 00000008 */ AptPrototype::getNativeHashVirtual(/* parameters unknown */) {}
/* 00241678 00000730 */ _malloc_r(/* parameters unknown */) {}
/* 001e7cd0 0000022c */ LobbyRegParams(/* parameters unknown */) {}
/* 00258178 00000008 */ EAGL::RenderContext::operator new [](/* parameters unknown */) {}
/* 00257028 00000024 */ AptValue::isScriptColour(/* parameters unknown */) {}
/* 002567f0 0000007c */ AptExtern::objectMemberSet(/* parameters unknown */) {}
/* 001eb220 00000058 */ TagFieldGetFlags(/* parameters unknown */) {}
/* 001b0f90 0000000c */ SHAPE_count(/* parameters unknown */) {}
/* 001f0180 000000a4 */ _HLBRosterSort(/* parameters unknown */) {}
/* 00257910 00000048 */ EASO::SharedLoginThreadInterface::Release(/* parameters unknown */) {}
/* 00254eb8 00000008 */ AptGlobal::operator delete [](/* parameters unknown */) {}
/* 001ee460 00000040 */ _listCreate(/* parameters unknown */) {}
/* 001e60e8 00000078 */ LobbyNameCmp(/* parameters unknown */) {}
/* 001dd8e0 0000001c */ NetTick(/* parameters unknown */) {}
/* 00251f40 0000001c */ AptObject::GetGCPointer(/* parameters unknown */) {}
/* 002520a8 00000008 */ AptObject::operator new [](/* parameters unknown */) {}
/* 001fadd0 00000128 */ _LobbyLocalizeSetup(/* parameters unknown */) {}
/* 0023efd8 00000028 */ _vfflush(/* parameters unknown */) {}
/* 001fc5e0 00000204 */ _BuddyApiHandleROSTMsg(/* parameters unknown */) {}
/* 001f1650 0000001c */ HLBListGetBuddyCountByFlags(/* parameters unknown */) {}
/* 0023aff0 00000094 */ _fwalk(/* parameters unknown */) {}
/* 001e30a0 00000184 */ LobbyApiAsyncMoveRoom(/* parameters unknown */) {}
/* 00252ed8 00000008 */ AptLookup::GetNumGCPointers(/* parameters unknown */) {}
/* 001ed8e8 00000030 */ LobbySettingCancel(/* parameters unknown */) {}
/* 001f3400 000000fc */ TourneyApiReqData(/* parameters unknown */) {}
/* 00254048 00000048 */ AptCIH::isTextInst(/* parameters unknown */) {}
/* 0023c6a8 0000033c */ _mbtowc_r(/* parameters unknown */) {}
/* 001f6440 000000a8 */ ProtoHttpGet(/* parameters unknown */) {}
/* 00252620 0000003c */ AptMouse::operator new(/* parameters unknown */) {}
/* 001fe0d8 0000002c */ BuddyApiBuddyInvite(/* parameters unknown */) {}
/* 001e06c8 000000a0 */ LobbyApiGlobalCallback(/* parameters unknown */) {}
/* 001de8d0 00000010 */ RpcAliveEE(/* parameters unknown */) {}
/* 001ef738 00000124 */ _HLBMsgListDeleteOldestMsg(/* parameters unknown */) {}
/* 001e0498 0000001c */ LobbyApiDisconnect(/* parameters unknown */) {}
/* 001fe210 00000008 */ BuddyApiJoinGame(/* parameters unknown */) {}
/* 00254320 00000184 */ AptTextFormat::AptTextFormat(/* parameters unknown */) {}
/* 002420a0 0000016c */ _malloc_trim_r(/* parameters unknown */) {}
/* 001e33f8 00000024 */ LobbyApiListGetHash(/* parameters unknown */) {}
/* 00254550 0000003c */ AptTextFormat::operator new(/* parameters unknown */) {}
/* 001e97f0 000000f8 */ LobbyStatbookFetchView(/* parameters unknown */) {}
/* 00255b28 000000e8 */ AptSavedInputCheckpoints::~AptSavedInputCheckpoints(/* parameters unknown */) {}
/* 001ee4c0 00000020 */ _listIsDirty(/* parameters unknown */) {}
/* 002521c0 00000008 */ AptMathObj::getNativeHashVirtual(/* parameters unknown */) {}
/* 00252b30 00000008 */ AptString::GetNext(/* parameters unknown */) {}
/* 0025b080 00000060 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator=(/* parameters unknown */) {}
/* 002582a8 00000008 */ EAGL::TextureRenderContext::operator new(/* parameters unknown */) {}
/* 00256b10 0000000c */ AptValue::getVtblIndex(/* parameters unknown */) {}
/* 0023a194 0000014c */ strcmp(/* parameters unknown */) {}
/* 001f5e98 000001c4 */ ProtoAriesPeek(/* parameters unknown */) {}
/* 00257108 00000024 */ AptValue::isTextFormat(/* parameters unknown */) {}
/* 001ee6d8 00000048 */ _HLBDestroyBuddy(/* parameters unknown */) {}
/* 00257f38 0000009c */ EAGLInternal::GifState::SetDepthTest(/* parameters unknown */) {}
/* 001f2680 0000003c */ HLBBudGetGameInviteFlags(/* parameters unknown */) {}
/* 001f5af8 0000008c */ ProtoAriesUnlisten(/* parameters unknown */) {}
/* 00257168 0000000c */ EAStringC::operator char *(/* parameters unknown */) {}
/* 00254958 00000008 */ AptDate::operator delete [](/* parameters unknown */) {}
/* 00254de8 0000001c */ AptGlobal::GetNumGCPointers(/* parameters unknown */) {}
/* 00252e68 00000008 */ AptRegister::GetNumGCPointers(/* parameters unknown */) {}
/* 002404d8 00000030 */ vfiprintf(/* parameters unknown */) {}
/* 001dd6b8 00000080 */ SockaddrInParse(/* parameters unknown */) {}
/* 002525c8 00000008 */ AptMouse::getNativeHash(/* parameters unknown */) {}
/* 001eb450 00000114 */ TagFieldSetToken(/* parameters unknown */) {}
/* 00256d08 00000008 */ AptValue::c_prototype(/* parameters unknown */) {}
/* 001f97d0 0000002c */ WebOfferSetPromo(/* parameters unknown */) {}
/* 001f7ad8 0000017c */ _WebOfferAction(/* parameters unknown */) {}
/* 00256cb0 00000008 */ AptValue::c_scriptfunction(/* parameters unknown */) {}
/* 001f62c0 0000001c */ ProtoAriesTick(/* parameters unknown */) {}
/* 00256b00 0000000c */ AptValue::getRefCount(/* parameters unknown */) {}
/* 00256cf8 00000008 */ AptValue::c_scriptcolour(/* parameters unknown */) {}
/* 001e5f98 00000088 */ HashNumDel(/* parameters unknown */) {}
/* 001f6c88 0000007c */ ProtoMangleConnect(/* parameters unknown */) {}
/* 002532e0 00000024 */ AptCharacterAnimationInst::operator delete(/* parameters unknown */) {}
/* 001f4940 0000011c */ TourneyApiGetUserByIdent(/* parameters unknown */) {}
/* 001f0a18 000000b4 */ HLBApiUpdate(/* parameters unknown */) {}
/* 001e9a98 0000007c */ LobbyStatbookRowData(/* parameters unknown */) {}
/* 00253f38 0000005c */ AptCIH::isCharacterInst(/* parameters unknown */) {}
/* 001ee528 0000001c */ _listItemGetByIndex(/* parameters unknown */) {}
/* 001b17f8 00000038 */ SHAPE_isclut(/* parameters unknown */) {}
/* 00257ad0 00000008 */ EASO::MySharedLoginAllocator::AddRef(/* parameters unknown */) {}
/* 001f20e0 0000003c */ HLBApiPresenceSame(/* parameters unknown */) {}
/* 002560a8 00000054 */ AptBoolean::~AptBoolean(/* parameters unknown */) {}
/* 001e4d08 00000070 */ LobbyApiColor(/* parameters unknown */) {}
/* 00253360 00000024 */ AptFile::operator delete(/* parameters unknown */) {}
/* 0025a648 00000060 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator=(/* parameters unknown */) {}
/* 0025bbf0 00000034 */ {anonymous}::CFESharedLoginAllocator::Alloc(/* parameters unknown */) {}
/* 002427e0 000001ec */ sn_floor(/* parameters unknown */) {}
/* 001ecc70 00000430 */ TagFieldPrintf(/* parameters unknown */) {}
/* 0023b490 00000080 */ sprintf(/* parameters unknown */) {}
/* 001e04b8 00000100 */ LobbyApiSuspend(/* parameters unknown */) {}
/* 001f6230 0000008c */ ProtoAriesSetKey(/* parameters unknown */) {}
/* 001e56d8 00000144 */ DispListOrder(/* parameters unknown */) {}
/* 00253768 0000001c */ EAStringC::EAStringC(/* parameters unknown */) {}
/* 001fca20 000000c8 */ _processAck(/* parameters unknown */) {}
/* 001f4ca8 00000188 */ TourneyApiGetGameByRound(/* parameters unknown */) {}
/* 001f25b0 00000078 */ HLBApiSetEmailForwarding(/* parameters unknown */) {}
/* 00255748 00000240 */ AptNativeHash::HashFindKey(/* parameters unknown */) {}
/* 001fe8a8 0000000c */ BuddyApiPresNoText(/* parameters unknown */) {}
/* 00257640 00000014 */ EAGL::TARExtension::GetHeight(/* parameters unknown */) {}
/* 001e5820 00000008 */ DispListDataSet(/* parameters unknown */) {}
/* 00256c20 00000010 */ AptValue::incGCRoot(/* parameters unknown */) {}
/* 00252848 0000006c */ EAStringC::~EAStringC(/* parameters unknown */) {}
/* 001f13e8 000000b4 */ HLBMsgListDeleteAll(/* parameters unknown */) {}
/* 0023c458 00000098 */ atexit(/* parameters unknown */) {}
/* 002570b8 00000024 */ AptValue::isPrototype(/* parameters unknown */) {}
/* 001e5618 0000003c */ DispListIndex(/* parameters unknown */) {}
/* 00243840 00000060 */ _write_r(/* parameters unknown */) {}
/* 002520b8 00000078 */ AptObject::~AptObject(/* parameters unknown */) {}
/* 00258140 00000024 */ EAGL::RenderContext::operator delete [](/* parameters unknown */) {}
/* 001fd6d0 00000008 */ BuddyApiResource(/* parameters unknown */) {}
/* 00244b80 00000198 */ _b2d(/* parameters unknown */) {}
/* 001f5818 00000148 */ ProtoAriesConnect(/* parameters unknown */) {}
/* 001f4408 00000098 */ TourneyApiClone(/* parameters unknown */) {}
/* 001f2628 00000054 */ HLBApiGetEmailForwarding(/* parameters unknown */) {}
/* 001b0e68 00000028 */ SHAPE_destroy(/* parameters unknown */) {}
/* 0025b8e0 00000008 */ EASO::NetLobby::getFindUserApi(/* parameters unknown */) {}
/* 001f1058 000000f0 */ HLBBudJoinGame(/* parameters unknown */) {}
/* 001ef3c8 000000fc */ _removeOldestTempBuddy(/* parameters unknown */) {}
/* 00242698 0000002c */ setlocale(/* parameters unknown */) {}
/* 001fbb00 0000008c */ _BuddyApiFlagsFromAttr(/* parameters unknown */) {}
/* 001fe8c0 00000008 */ BuddyApiResumeXDK(/* parameters unknown */) {}
/* 0025b8d8 00000008 */ EASO::NetLobby::GetNetLobby(/* parameters unknown */) {}
/* 001f1e18 00000038 */ HLBListRemoveFromGroup(/* parameters unknown */) {}
/* 00258118 00000024 */ EAGL::RenderContext::operator new [](/* parameters unknown */) {}
/* 00251f20 0000001c */ AptObject::GetNumGCPointers(/* parameters unknown */) {}
/* 001f06d0 00000098 */ HLBApiCreate2(/* parameters unknown */) {}
/* 001fa380 0000015c */ LobbyZone(/* parameters unknown */) {}
/* 0025bab8 00000008 */ EASO::NetLobby::SetMangleTimeout(/* parameters unknown */) {}
/* 00253118 0000001c */ AptFrameStack::GetGCPointer(/* parameters unknown */) {}
/* 001e5f40 00000058 */ HashNumFind(/* parameters unknown */) {}
/* 00252b48 000000b8 */ AptString::Create(/* parameters unknown */) {}
/* 00257808 00000064 */ EASO::SharedLoginThreadInterface::~SharedLoginThreadInterface(/* parameters unknown */) {}
/* 00255140 00000024 */ AptCharacterButtonInst::operator new [](/* parameters unknown */) {}
/* 00256da8 00000014 */ AptValue::isNone(/* parameters unknown */) {}
/* 00253308 00000024 */ AptCharacterAnimationInst::operator new [](/* parameters unknown */) {}
/* 002523a0 0000001c */ AptKey::DestroyGCPointers(/* parameters unknown */) {}
/* 00252400 0000001c */ AptKey::GetGCPointer(/* parameters unknown */) {}
/* 002530f8 0000001c */ AptFrameStack::GetNumGCPointers(/* parameters unknown */) {}
/* 0025b9a8 0000001c */ EASO::NetLobby::SetCategoryUrl(/* parameters unknown */) {}
/* 002572b0 00000084 */ AptCharacterMorphInst::~AptCharacterMorphInst(/* parameters unknown */) {}
/* 00255008 00000008 */ AptScriptColour::operator delete [](/* parameters unknown */) {}
/* 001dd900 00000080 */ NetCompare(/* parameters unknown */) {}
/* 00243448 000001c0 */ sn_log10(/* parameters unknown */) {}
/* 001fde20 00000084 */ _BuddyApiValidateName(/* parameters unknown */) {}
/* 001fe8c8 00000108 */ BuddyApiPresSend(/* parameters unknown */) {}
/* 001ee630 000000a4 */ _countBuddies(/* parameters unknown */) {}
/* 001fd578 000000e4 */ BuddyApiCreate2(/* parameters unknown */) {}
/* 00256ce8 00000008 */ AptValue::c_global(/* parameters unknown */) {}
/* 001e4ee0 0000002c */ LobbyApiGetTrophyCount(/* parameters unknown */) {}
/* 001df310 00000060 */ NetConnDisconnect(/* parameters unknown */) {}
/* 001fa4e0 00000044 */ LobbyHash(/* parameters unknown */) {}
/* 00257158 0000000c */ EAStringC::c_str(/* parameters unknown */) {}
/* 001ea690 00000048 */ TagFieldDupl(/* parameters unknown */) {}
/* 001fdb58 000000d0 */ BuddyApiFlush(/* parameters unknown */) {}
/* 00256e70 00000024 */ AptValue::isRegister(/* parameters unknown */) {}
/* 001f1dd0 00000008 */ HLBListDeleteTempBuddies(/* parameters unknown */) {}
/* 0025b940 00000008 */ EASO::NetLobby::GetLKey(/* parameters unknown */) {}
/* 00251df0 00000008 */ AptArray::operator new [](/* parameters unknown */) {}
/* 001e03d0 000000c8 */ LobbyApiConnect(/* parameters unknown */) {}
/* 002576b0 00000008 */ Realmc::IThread::AddRef(/* parameters unknown */) {}
/* 001f1bb8 00000040 */ HLBListFlagTempBuddy(/* parameters unknown */) {}
/* 00251f88 0000001c */ AptObject::Set(/* parameters unknown */) {}
/* 001f0c98 0000001c */ HLBApiUserFound(/* parameters unknown */) {}
/* 001f26e0 0000001c */ HLBApiGetMyTitleName(/* parameters unknown */) {}
/* 0025bac8 00000008 */ EASO::NetLobby::SetMangleConnTimeout(/* parameters unknown */) {}
/* 001ee790 00000088 */ _preConnTempBudListDestroy(/* parameters unknown */) {}
/* 001f9cc8 00000048 */ DirtyAddrGetLocalAddr(/* parameters unknown */) {}
/* 001f5440 0000002c */ TourneyApiFlagClear(/* parameters unknown */) {}
/* 001e1058 000000dc */ LobbyApiGetUserInfo(/* parameters unknown */) {}
/* 001eef50 00000238 */ _addChangeLLBud(/* parameters unknown */) {}
/* 001f5048 0000002c */ TourneyApiGetGameListByUser(/* parameters unknown */) {}
/* 002521c8 00000008 */ AptMathObj::getNativeHash(/* parameters unknown */) {}
/* 00252ae0 0000001c */ AptString::cat(/* parameters unknown */) {}
/* 001e3b00 00000180 */ LobbyApiRoomDefault(/* parameters unknown */) {}
/* 001f2358 00000258 */ HLBListSendChatMsg(/* parameters unknown */) {}
/* 001e6020 0000000c */ HasherEnumInit(/* parameters unknown */) {}
/* 001b0418 00000030 */ FONT_destroy(/* parameters unknown */) {}
/* 00253278 0000003c */ AptCharacterInst::DestroyGCPointers(/* parameters unknown */) {}
/* 0025bb68 00000008 */ EASO::EasoUtil::GetTMID(/* parameters unknown */) {}
/* 00256700 00000008 */ AptValue::PreDestroy(/* parameters unknown */) {}
/* 001e1020 00000034 */ LobbyApiRoomEntry(/* parameters unknown */) {}
/* 001b09b8 00000110 */ SHAPE_createsize(/* parameters unknown */) {}
/* 00256878 00000008 */ AptExtern::GetGCPointer(/* parameters unknown */) {}
/* 001e4510 00000134 */ LobbyApiExtractRankRecord(/* parameters unknown */) {}
/* 001f8110 00000120 */ WebOfferExecute(/* parameters unknown */) {}
/* 002520b0 00000008 */ AptObject::operator delete [](/* parameters unknown */) {}
/* 001ea280 00000020 */ TagFieldDivider(/* parameters unknown */) {}
/* 0025b818 000000bc */ void EASO::AddPair<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> >(/* parameters unknown */) {}
/* 00255e28 00000014 */ AptInteger::Destroy(/* parameters unknown */) {}
/* 00256e48 00000024 */ AptValue::isInteger(/* parameters unknown */) {}
/* 001fe828 0000004c */ BuddyApiPresDiff(/* parameters unknown */) {}
/* 002426c8 00000024 */ localeconv(/* parameters unknown */) {}
/* 00257598 0000003c */ AptXmlAttributes::operator new(/* parameters unknown */) {}
/* 001f2c00 0000000c */ TourneyApiDebug(/* parameters unknown */) {}
/* 002440a0 00000108 */ _multadd(/* parameters unknown */) {}
/* 00256880 0000005c */ AptExtern::~AptExtern(/* parameters unknown */) {}
/* 001e5cd0 00000080 */ HashStrFind(/* parameters unknown */) {}
/* 0025bb28 00000008 */ EASO::NetLobby::GetScoreReportName1(/* parameters unknown */) {}
/* 001fbfc8 000003bc */ _BuddyApiPresRecv(/* parameters unknown */) {}
/* 002582b8 00000008 */ EAGL::TextureRenderContext::operator new [](/* parameters unknown */) {}
/* 001f2300 00000054 */ HLBApiPresenceSendSetPresence(/* parameters unknown */) {}
/* 002530e8 00000008 */ AptFrameStack::getNativeHashVirtual(/* parameters unknown */) {}
/* 00251c90 00000020 */ AptArray::GetAt(/* parameters unknown */) {}
/* 002525c0 00000008 */ AptMouse::getNativeHashVirtual(/* parameters unknown */) {}
/* 001f07a8 000000a8 */ HLBApiOverrideConstants(/* parameters unknown */) {}
/* 00257ee8 00000050 */ EAGLInternal::GifState::SetDepthTestMethod(/* parameters unknown */) {}
/* 001f44a0 00000024 */ TourneyApiFree(/* parameters unknown */) {}
/* 0025b8f0 0000001c */ EASO::NetLobby::SetPersona(/* parameters unknown */) {}
/* 00252930 000000a8 */ AptNativeHash::SetProtoGC(/* parameters unknown */) {}
/* 00254590 00000044 */ AptTextFormat::operator delete(/* parameters unknown */) {}
/* 0025b8e8 00000008 */ EASO::NetLobby::isXboxVoiceBanned(/* parameters unknown */) {}
/* 00256018 00000008 */ AptSound::operator delete [](/* parameters unknown */) {}
/* 002522a8 00000008 */ AptMathObj::operator new [](/* parameters unknown */) {}
/* 001fde18 00000008 */ BuddyApiRosterList(/* parameters unknown */) {}
/* 00256780 0000005c */ AptNone::~AptNone(/* parameters unknown */) {}
/* 001e5610 00000008 */ DispListShown(/* parameters unknown */) {}
/* 001f9c70 00000058 */ DirtyAddrFromHostAddr(/* parameters unknown */) {}
/* 001ef930 000001e0 */ _HLBMsgListAddMsg(/* parameters unknown */) {}
/* 001f1538 00000064 */ HLBListGetBuddyByIndex(/* parameters unknown */) {}
/* 001b0e90 00000048 */ SHAPE_cluttype(/* parameters unknown */) {}
/* 001fe9d0 0000003c */ BuddyApiSend(/* parameters unknown */) {}
/* 002566a8 00000008 */ AptValue::getNativeHashVirtual(/* parameters unknown */) {}
/* 001f8440 00000024 */ WebOfferParamList(/* parameters unknown */) {}
/* 001e5608 00000008 */ DispListCount(/* parameters unknown */) {}
/* 001e5258 00000074 */ DispListCreate(/* parameters unknown */) {}
/* 00255ee8 00000008 */ AptScriptFunction::getFunction(/* parameters unknown */) {}
/* 001f6670 0000015c */ ProtoHttpPost(/* parameters unknown */) {}
/* 001dd368 000000b4 */ SockaddrInSetAddrText(/* parameters unknown */) {}
/* 00257628 00000008 */ AptXmlAttributes::operator delete [](/* parameters unknown */) {}
/* 00252d58 0000010c */ AptActionInterpreter::~AptActionInterpreter(/* parameters unknown */) {}
/* 0025b478 0000006c */ void EASO::AddPair<unsigned int>(/* parameters unknown */) {}
/* 001efb10 00000440 */ _HLBListAddChangeBuddy(/* parameters unknown */) {}
/* 00258070 00000008 */ EAGL::RenderContextBase::GetObjectType(/* parameters unknown */) {}
/* 001fcca8 00000130 */ _BuddyApiSendResponse(/* parameters unknown */) {}
/* 0025ba48 0000001c */ EASO::NetLobby::SetFaqServer(/* parameters unknown */) {}
/* 00251c78 00000018 */ AptArray::IsValueGC(/* parameters unknown */) {}
/* 001f0d60 00000088 */ HLBBudIsTemporary(/* parameters unknown */) {}
/* 001e5df8 000000e0 */ HashNumAdd(/* parameters unknown */) {}
/* 001eda50 00000034 */ LobbySettingGetNumber(/* parameters unknown */) {}
/* 0025ba18 00000008 */ EASO::NetLobby::GetNewsServer(/* parameters unknown */) {}
/* 00257620 00000008 */ AptXmlAttributes::operator new [](/* parameters unknown */) {}
/* 001e5c40 00000090 */ HashStrReplace(/* parameters unknown */) {}
/* 00256948 0000001c */ AptGlobalExtensionObject::GetGCPointer(/* parameters unknown */) {}
/* 001e7f00 00000610 */ LobbyRegSubmitCB(/* parameters unknown */) {}
/* 002530c0 0000001c */ AptFrameStack::DestroyGCPointers(/* parameters unknown */) {}
/* 001f2700 00000008 */ HLBListSetGameInviteSessionID(/* parameters unknown */) {}
/* 00252cf0 00000064 */ EAStringC::Clear(/* parameters unknown */) {}
/* 001edab0 0000001c */ LobbySettingDestroy(/* parameters unknown */) {}
/* 001eac58 000000bc */ TagFieldGetRaw(/* parameters unknown */) {}
/* 001ee970 000000dc */ _preConnTempBudListLoad(/* parameters unknown */) {}
/* 001fcdd8 00000150 */ _BuddyApiAddOrSetFlags(/* parameters unknown */) {}
/* 00256ca0 00000008 */ AptValue::c_string(/* parameters unknown */) {}
/* 00200ff0 000001bc */ CryptMD5Final(/* parameters unknown */) {}
/* 00258170 00000008 */ EAGL::RenderContext::operator delete(/* parameters unknown */) {}
/* 001ecb78 000000f8 */ TagFieldGetFloat(/* parameters unknown */) {}
/* 001e8510 00000088 */ LobbyRegAbort(/* parameters unknown */) {}
/* 001f0098 00000074 */ HLBListDisableSorting(/* parameters unknown */) {}
/* 001f4758 0000008c */ TourneyApiGetLatestNews(/* parameters unknown */) {}
/* 0025ba90 00000008 */ EASO::NetLobby::GetMangleServerPort(/* parameters unknown */) {}
/* 00258180 00000008 */ EAGL::RenderContext::operator delete [](/* parameters unknown */) {}
/* 0023b618 00000080 */ __swrite(/* parameters unknown */) {}
/* 00252810 00000008 */ AptLoadVars::operator new [](/* parameters unknown */) {}
/* 001eae60 00000134 */ TagFieldSetNumber64(/* parameters unknown */) {}
/* 001dd550 00000034 */ SocketHtonl(/* parameters unknown */) {}
/* 001e6490 000003cc */ PingManagerUpdate(/* parameters unknown */) {}
/* 001e98e8 0000014c */ LobbyStatbookFetch(/* parameters unknown */) {}
/* 001ee0b0 0000001c */ LobbyFindUserDestroy(/* parameters unknown */) {}
/* 001f3810 00000084 */ TourneyApiResult(/* parameters unknown */) {}
/* 001dd538 00000018 */ SocketHtons(/* parameters unknown */) {}
/* 001ff7e8 000001b4 */ BuddyApiRespondBuddy(/* parameters unknown */) {}
/* 001e33d0 00000024 */ LobbyApiListGetDisp(/* parameters unknown */) {}
/* 002525d0 00000008 */ AptMouse::ContainsNativeHashVirtual(/* parameters unknown */) {}
/* 00243bf0 00000044 */ fiprintf(/* parameters unknown */) {}
/* 001f09b8 0000005c */ HLBApiDestroy(/* parameters unknown */) {}
/* 0025b070 00000008 */ EASO::NetVOIP::SetLastPollTime(/* parameters unknown */) {}
/* 00240508 00000f04 */ _vfiprintf_r(/* parameters unknown */) {}
/* 001ea6d8 0000022c */ TagFieldMerge(/* parameters unknown */) {}
/* 001fb2f0 00000104 */ _LobbyLocalizeGetLocaleString(/* parameters unknown */) {}
/* 002577b0 00000054 */ EASO::MyMutex::~MyMutex(/* parameters unknown */) {}
/* 001f47e8 0000007c */ TourneyApiGetNewsCount(/* parameters unknown */) {}
/* 0025b548 000000e8 */ void EASO::AddPair<char *(*)>(/* parameters unknown */) {}
/* 00258168 00000008 */ EAGL::RenderContext::operator new(/* parameters unknown */) {}
/* 00244928 0000018c */ __mdiff(/* parameters unknown */) {}
/* 001b1168 000001d0 */ SHAPE_getpixeloffset(/* parameters unknown */) {}
/* 00256910 00000008 */ AptGlobalExtensionObject::getNativeHashVirtual(/* parameters unknown */) {}
/* 00252788 0000003c */ AptLoadVars::operator new(/* parameters unknown */) {}
/* 001b0f9c 00000028 */ SHAPE_pointer(/* parameters unknown */) {}
/* 00254090 00000048 */ AptCIH::isStaticTextInst(/* parameters unknown */) {}
/* 001f7aa0 00000034 */ _WebOfferActionExit(/* parameters unknown */) {}
/* 002582c0 00000008 */ EAGL::TextureRenderContext::operator delete [](/* parameters unknown */) {}
/* 0025b068 00000008 */ EASO::NetVOIP::GetLastPollTime(/* parameters unknown */) {}
/* 002567e8 00000008 */ AptExtern::Release(/* parameters unknown */) {}
/* 001f5470 00000058 */ TourneyApiFlagAdd(/* parameters unknown */) {}
/* 0025a6a8 00000154 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::Erase(/* parameters unknown */) {}
/* 001e5390 000000c0 */ DispListAdd(/* parameters unknown */) {}
/* 001f0850 00000008 */ HLBApiSetUtf8TransTbl(/* parameters unknown */) {}
/* 001e59b8 000000a0 */ HasherExpand(/* parameters unknown */) {}
/* 002531b0 00000020 */ EAStringC::EAStringC(/* parameters unknown */) {}
/* 001faf60 0000000c */ LobbyLocalizeSetCallback(/* parameters unknown */) {}
/* 00256760 00000008 */ AptNone::AddRef(/* parameters unknown */) {}
/* 00253e88 00000008 */ AptCIH::getSpriteInst(/* parameters unknown */) {}
/* 0023c230 000000f0 */ __sccl(/* parameters unknown */) {}
/* 0023c510 00000028 */ atoi(/* parameters unknown */) {}
/* 001ee500 00000024 */ _listItemAdd(/* parameters unknown */) {}
/* 002580c8 00000024 */ EAGL::RenderContext::operator delete(/* parameters unknown */) {}
/* 00256d30 00000008 */ AptValue::c_xml(/* parameters unknown */) {}
/* 0025bb50 00000008 */ EASO::EasoUtil::GetHomeScore(/* parameters unknown */) {}
/* 001f0ef0 00000014 */ HLBBudIsAvailableForChat(/* parameters unknown */) {}
/* 00252b28 00000008 */ AptString::SetNext(/* parameters unknown */) {}
/* 002525e0 0000001c */ AptMouse::GetNumGCPointers(/* parameters unknown */) {}
/* 001ff9a0 000001c0 */ BuddyApiRespondGame(/* parameters unknown */) {}
/* 00256c88 00000008 */ AptValue::c_integer(/* parameters unknown */) {}
/* 001ed800 00000074 */ LobbySettingLoad(/* parameters unknown */) {}
/* 00252398 00000008 */ AptKey::Release(/* parameters unknown */) {}
/* 002531f8 0000007c */ AptIntervalTimer::cleanParams(/* parameters unknown */) {}
/* 001f1e50 0000006c */ HLBListClearGroup(/* parameters unknown */) {}
/* 00256ca8 00000008 */ AptValue::c_boolean(/* parameters unknown */) {}
/* 00256f60 00000024 */ AptValue::isArray(/* parameters unknown */) {}
/* 00251eb0 00000024 */ AptObject::setHasClass(/* parameters unknown */) {}
/* 001fd008 000001e4 */ _BuddyApiHandleGNOTMsg(/* parameters unknown */) {}
/* 00254c78 00000014 */ AptFloat::Destroy(/* parameters unknown */) {}
/* 00258188 0000000c */ EAGL::RenderContext::GetCurrentFrameNumber(/* parameters unknown */) {}
/* 002521d8 00000008 */ AptMathObj::IsGarbageCollected(/* parameters unknown */) {}
/* 001ffb70 00000040 */ BuddyApiGetMyTitleName(/* parameters unknown */) {}
/* 0023e840 0000007c */ strupr(/* parameters unknown */) {}
/* 00243ce0 00000030 */ memcpyalpha(/* parameters unknown */) {}
/* 001eef20 00000030 */ _setUpOp(/* parameters unknown */) {}
/* 0023aa88 000000dc */ _fopen_r(/* parameters unknown */) {}
/* 001fa378 00000008 */ LobbyTime(/* parameters unknown */) {}
/* 001f5508 0000004c */ TourneyApiFlagSet(/* parameters unknown */) {}
/* 002521a0 0000001c */ AptMathObj::DestroyGCPointers(/* parameters unknown */) {}
/* 002562d0 00000024 */ AptBoolean::operator new(/* parameters unknown */) {}
/* 00256bb8 00000028 */ AptValue::setGCMark(/* parameters unknown */) {}
/* 001e4648 000001e4 */ LobbyApiExtractRoomRecord(/* parameters unknown */) {}
/* 001f63d0 00000070 */ ProtoHttpDestroy(/* parameters unknown */) {}
/* 00254dd8 00000008 */ AptGlobal::ContainsNativeHashVirtual(/* parameters unknown */) {}
/* 001f3520 000000a4 */ TourneyApiReqUpdate(/* parameters unknown */) {}
/* 00252c78 00000024 */ AptString::operator delete(/* parameters unknown */) {}
/* 001fbc08 0000005c */ _createBuddyDispList(/* parameters unknown */) {}
/* 001f0858 00000034 */ HLBApiInitialize(/* parameters unknown */) {}
/* 0025b040 00000008 */ EASO::NetVOIP::GetCurrentHardwareFlags(/* parameters unknown */) {}
/* 001b0530 0000002c */ FONTDL_builda(/* parameters unknown */) {}
/* 00251e00 00000048 */ AptValueVector::PushValue(/* parameters unknown */) {}
/* 0023a3f8 00000144 */ strlen(/* parameters unknown */) {}
/* 00254ec0 00000078 */ AptGlobal::~AptGlobal(/* parameters unknown */) {}
/* 001f7008 0000009c */ ProtoPingCreate(/* parameters unknown */) {}
/* 002550f0 00000024 */ AptCharacterButtonInst::operator new(/* parameters unknown */) {}
/* 00252e78 0000005c */ AptRegister::~AptRegister(/* parameters unknown */) {}
/* 00256b70 0000001c */ AptValue::setVtblIndex(/* parameters unknown */) {}
/* 00257388 0000014c */ AptCharacterTextInst::~AptCharacterTextInst(/* parameters unknown */) {}
/* 001e9b50 00000028 */ LobbyStatbookViewInfo(/* parameters unknown */) {}
/* 001fb8d8 000000a4 */ Utf8EncodeFrom8Bit(/* parameters unknown */) {}
/* 001ffb60 00000008 */ _BuddyApiSetTalkToXbox(/* parameters unknown */) {}
/* 002523c8 00000008 */ AptKey::getNativeHash(/* parameters unknown */) {}
/* 00256cc0 00000008 */ AptValue::c_cih(/* parameters unknown */) {}
/* 001f0938 0000001c */ HLBApiRegisterConnectCallback(/* parameters unknown */) {}
/* 0025b1a8 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, LobbyRegContextE>(/* parameters unknown */) {}
/* 001fd6e8 000000d0 */ BuddyApiConfig(/* parameters unknown */) {}
/* 0023c428 00000030 */ __assert(/* parameters unknown */) {}
/* 00256920 00000008 */ AptGlobalExtensionObject::IsGarbageCollected(/* parameters unknown */) {}
/* 001f9c00 0000006c */ DirtyAddrToHostAddr(/* parameters unknown */) {}
/* 001e6a50 00000060 */ PingManagerInvalidateCache(/* parameters unknown */) {}
/* 002576b8 00000050 */ EASO::MySharedLoginAllocator::Instance(/* parameters unknown */) {}
/* 00258038 0000000c */ EAGLInternal::VifListAccumulator::Reset(/* parameters unknown */) {}
/* 001e4d78 00000164 */ LobbyApiCheckRoomEntry(/* parameters unknown */) {}
/* 0025bc28 00000030 */ {anonymous}::CFESharedLoginAllocator::Free(/* parameters unknown */) {}
/* 00239ff4 000001a0 */ strchr(/* parameters unknown */) {}
/* 0023b220 00000168 */ __srefill(/* parameters unknown */) {}
/* 001f2f50 000000f8 */ TourneyApiReqNews(/* parameters unknown */) {}
/* 001e5930 00000008 */ HasherSetStrCompareFunc(/* parameters unknown */) {}
/* 00256098 00000008 */ AptBoolean::GetNumGCPointers(/* parameters unknown */) {}
/* 001de3e8 000000bc */ RpcLoaded(/* parameters unknown */) {}
/* 002550a0 00000050 */ AptCharacterButtonInst::AptCharacterButtonInst(/* parameters unknown */) {}
/* 001f69f8 00000094 */ ProtoHttpControl(/* parameters unknown */) {}
/* 001fe218 0000025c */ BuddyApiDel(/* parameters unknown */) {}
/* 001f1dd8 0000003c */ HLBListAddToGroup(/* parameters unknown */) {}
/* 001f7860 00000138 */ _WebOfferActionGoto(/* parameters unknown */) {}
/* 001e3050 00000050 */ LobbyApiAsyncProcess(/* parameters unknown */) {}
/* 001faef8 00000068 */ LobbyLocalizeCreate(/* parameters unknown */) {}
/* 0025bad8 00000008 */ EASO::NetLobby::SetChatMsgLimit(/* parameters unknown */) {}
/* 00258208 00000024 */ EAGL::TextureRenderContext::operator delete(/* parameters unknown */) {}
/* 001e48e0 00000338 */ LobbyApiExtractUserRecord(/* parameters unknown */) {}
/* 001f5d20 00000158 */ ProtoAriesSend(/* parameters unknown */) {}
/* 00256b40 0000000c */ AptValue::getIsDefined(/* parameters unknown */) {}
/* 001e6030 000000b4 */ HasherEnum(/* parameters unknown */) {}
/* 002581b8 00000024 */ EAGL::TextureRenderContext::operator new(/* parameters unknown */) {}
/* 00258280 00000024 */ EAGL::TextureRenderContext::operator delete [](/* parameters unknown */) {}
/* 001fcb80 00000128 */ _BuddyApiSendMsg(/* parameters unknown */) {}
/* 001e8828 00000274 */ LobbyRegValidate(/* parameters unknown */) {}
/* 001e69b0 0000002c */ PingManagerPingServer(/* parameters unknown */) {}
/* 0023f1d8 00001044 */ _vfprintf_r(/* parameters unknown */) {}
/* 001e1018 00000008 */ LobbyApiDebug(/* parameters unknown */) {}
/* 001e6390 000000b4 */ PingManagerCreate(/* parameters unknown */) {}
/* 0025bb48 00000008 */ EASO::EasoUtil::GetAwayUser(/* parameters unknown */) {}
/* 001fb798 0000008c */ Utf8EncodeFromUCS2(/* parameters unknown */) {}
/* 001b05d0 0000007c */ FONT_drawtextfa(/* parameters unknown */) {}
/* 001f1960 00000098 */ HLBListAnswerGameInvite(/* parameters unknown */) {}
/* 001ed438 00000124 */ TagFieldFindNext(/* parameters unknown */) {}
/* 001fbc68 0000009c */ _BuddyApiExtractRandomSeed(/* parameters unknown */) {}
/* 0025b360 0000002c */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy> EASO::String::LexicalCastNamespace::LexicalCast<EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>, unsigned int>(/* parameters unknown */) {}
/* 001f2708 00000008 */ HLBListGetGameSessionID(/* parameters unknown */) {}
/* 00257a80 0000002c */ EASO::MySharedLoginAllocator::Alloc(/* parameters unknown */) {}
/* 001f8468 00000148 */ WebOfferAction(/* parameters unknown */) {}
/* 0025a530 00000054 */ EASO::EasoHub::Instance(/* parameters unknown */) {}
/* 00256d10 00000008 */ AptValue::c_date(/* parameters unknown */) {}
/* 001ee2b0 000000b4 */ _msgTypeToOp(/* parameters unknown */) {}
/* 001dea38 0000001c */ NetConnElapsed(/* parameters unknown */) {}
/* 001f6f00 00000008 */ ProtoMangleDisconnect(/* parameters unknown */) {}
/* 0025b9f8 0000001c */ EASO::NetLobby::SetManglerServer(/* parameters unknown */) {}
/* 001f1360 0000001c */ HLBMsgListGetTotalCount(/* parameters unknown */) {}
/* 00253ec8 00000008 */ AptCIH::getCharacterInst(/* parameters unknown */) {}
/* 001b0ed8 0000001c */ SHAPE_depth(/* parameters unknown */) {}
/* 00257de0 0000001c */ EAGLInternal::GifState::SetFogging(/* parameters unknown */) {}
/* 001f5558 0000002c */ TourneyApiFlagGetMask(/* parameters unknown */) {}
/* 0025b950 0000001c */ EASO::NetLobby::setLKey(/* parameters unknown */) {}
/* 00252060 00000044 */ AptObject::operator delete(/* parameters unknown */) {}
/* 001fe4d8 0000010c */ BuddyApiFindUsers(/* parameters unknown */) {}
/* 00257338 00000050 */ AptCharacterSpriteInst::~AptCharacterSpriteInst(/* parameters unknown */) {}
/* 0025b9f0 00000008 */ EASO::NetLobby::GetMangleServer(/* parameters unknown */) {}
/* 00256d40 00000008 */ AptValue::c_loadvars(/* parameters unknown */) {}
/* 001e8598 000000e0 */ LobbyRegAlert(/* parameters unknown */) {}
/* 001e1138 00000074 */ LobbyApiGetUserSetInfo(/* parameters unknown */) {}
/* 00257588 00000008 */ AptXmlNode::operator new [](/* parameters unknown */) {}
/* 001dedc0 000000c4 */ NetConnStartup(/* parameters unknown */) {}
/* 0025bd00 000000ac */ EACSL::Message::Reset(/* parameters unknown */) {}
/* 001f9800 000000a8 */ WebOfferGetMenu(/* parameters unknown */) {}
/* 00253ed0 00000064 */ AptCIH::isSpriteInst(/* parameters unknown */) {}
/* 002567e0 00000008 */ AptExtern::AddRef(/* parameters unknown */) {}
/* 001e9a70 00000028 */ LobbyStatbookRowInfo(/* parameters unknown */) {}
/* 00257b58 00000010 */ EASO::MyMutex::AddRef(/* parameters unknown */) {}
/* 001f0bf0 00000044 */ HLBApiCancelOp(/* parameters unknown */) {}
/* 001eb278 00000160 */ TagFieldSetAddress(/* parameters unknown */) {}
/* 001f2d08 00000058 */ TourneyApiReqRaw(/* parameters unknown */) {}
/* 001eedb8 00000094 */ _constructHLBud(/* parameters unknown */) {}
/* 0025b050 00000008 */ EASO::NetVOIP::IsNextDisconnectSilent(/* parameters unknown */) {}
/* 0025ba70 0000001c */ EASO::NetLobby::SetTosServer(/* parameters unknown */) {}
/* 0025a000 0000006c */ void EASO::AddPair<char *>(/* parameters unknown */) {}
/* 00259890 000000c4 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::BasicString(/* parameters unknown */) {}
/* 00256020 00000078 */ AptSound::~AptSound(/* parameters unknown */) {}
/* 001fb980 000000c8 */ Utf8TranslateTo8Bit(/* parameters unknown */) {}
/* 001f0650 0000007c */ HLBApiCreate(/* parameters unknown */) {}
/* 002579d0 00000038 */ EASO::SharedLoginThreadInterface::WaitForEnd(/* parameters unknown */) {}
/* 001f70a8 00000070 */ ProtoPingDestroy(/* parameters unknown */) {}
/* 00257130 00000024 */ AptValue::isMovieClip(/* parameters unknown */) {}
/* 001eaaa8 00000044 */ TagFieldFindIdx(/* parameters unknown */) {}
/* 00202f20 00000000 */ DirtyDnasUpdate(/* parameters unknown */) {}
/* 001e5d50 000000a8 */ HashStrDel(/* parameters unknown */) {}
/* 0023f108 000000cc */ vfprintf(/* parameters unknown */) {}
/* 002537d0 00000574 */ void EA::String::BasicString<StringAsVectorEncoding<AptSharedPtr<AptFile> >, StringAsVectorPolicy>::Insert<AptSharedPtr<AptFile> *>(/* parameters unknown */) {}
/* 001f6a90 00000090 */ ProtoHttpStatus(/* parameters unknown */) {}
/* 001e9b18 00000030 */ LobbyStatbookViewStatus(/* parameters unknown */) {}
/* 00256ce0 00000008 */ AptValue::c_mouse(/* parameters unknown */) {}
/* 001e87e8 00000014 */ LobbyRegGetContext(/* parameters unknown */) {}
/* 00244768 00000154 */ _lshift(/* parameters unknown */) {}
/* 001f21a0 00000054 */ HLBApiPresenceOffline(/* parameters unknown */) {}
/* 0025b0e0 00000094 */ EASO::String::BasicString<EASO::String::EncodingUtf8, EASO::FifaStringPolicy>::operator+=(/* parameters unknown */) {}
/* 001ead18 00000148 */ TagFieldSetNumber(/* parameters unknown */) {}
/* 0023c660 00000048 */ free(/* parameters unknown */) {}
/* 00252420 0000003c */ AptKey::operator new(/* parameters unknown */) {}
/* 00254280 0000003c */ AptCIH::operator new(/* parameters unknown */) {}
/* 0025baa0 00000008 */ EASO::NetLobby::GetMangleTestTimeout(/* parameters unknown */) {}
/* 00244068 00000034 */ _Bfree(/* parameters unknown */) {}
/* 001dd420 00000118 */ SockaddrInGetAddrText(/* parameters unknown */) {}
/* 00252260 00000044 */ AptMathObj::operator delete(/* parameters unknown */) {}
/* 001f0970 00000048 */ HLBApiDisconnect(/* parameters unknown */) {}
/* 001def10 00000070 */ NetConnConnect(/* parameters unknown */) {}
/* 001ee368 0000001c */ _getRost(/* parameters unknown */) {}
/* 00257d08 00000010 */ EAGLInternal::PropertyParser::MAXINT(/* parameters unknown */) {}
