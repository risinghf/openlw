import os
import glob

region_band_list = [
'REGION_CN470OLD',
'REGION_EU868',
#'REGION_AS923',
#'REGION_AU915',
#'REGION_CN470',
#'REGION_CN779',
#'REGION_EU433',
#'REGION_KR920',
#'REGION_IN865',
#'REGION_US915',
#'REGION_RU864',
]
Export('region_band_list')

module_list_mcu_l0xx = [
'RHF0M063',
'RHF3M076',
]

module_list_mcu_wlxx = [
'RHF0M0E5',
'RHF0M003HF22',
'RHF0M003LF22',
]

module_list_use_tcxo = [
'RHF0M0E5',
'RHF0M003HF22',
]

module_list_radio_126x = [
'RHF0M063',
'RHF0M0E5',
'RHF0M003HF22',
'RHF0M003LF22',
]
# scons list
if 'list' in COMMAND_LINE_TARGETS:
    print('Suppor module list:')
    for module in module_list_mcu_l0xx:
        print('- ' + module)
    for module in module_list_mcu_wlxx:
        print('- ' + module)
    exit()

argc = len(ARGUMENTS)
if(argc != 2):
    print('Error: Missing parameters')
    print('Param1: [debug]   --> 0/1')
    print('Param2: [project] --> Use the [scons list] command to find supported modules')
    print('Example: scons debug=1 project=RHF0M003HF22')
    exit()

SCON_PROJECT_NAME = ARGUMENTS.get('project').lower()

if (SCON_PROJECT_NAME.upper() not in module_list_mcu_l0xx) and \
   (SCON_PROJECT_NAME.upper() not in module_list_mcu_wlxx):
   print('Please select right module: Use [scons list] to query')
   exit()
Export('SCON_PROJECT_NAME')
print(SCON_PROJECT_NAME)

# radio select
RADIO_TYPE = 'sx1276'
if SCON_PROJECT_NAME.upper() in module_list_radio_126x:
    RADIO_TYPE = 'sx126x'
Export('RADIO_TYPE')

# project config
LINKER_SCRIPT = f'src/boards/{SCON_PROJECT_NAME.upper()}/gcc'
LINK_FILE = glob.glob(os.path.join(LINKER_SCRIPT, '*.ld'))

if len(LINK_FILE) != 1:
    print('Error link file too many, just only one')
    exit()
LINKER_SCRIPT = LINK_FILE[0]
if os.path.exists(LINKER_SCRIPT):
    print('Link script exists path: ' + LINKER_SCRIPT )
else:
    print('Link script not exists')
    exit()

if ARGUMENTS.get('debug') == '1':
    BUILD = 'Debug'
else:
    BUILD = 'Release'

# toolchains
if os.getenv('RHF_TOOL_CHAINS'):
    TOOL_PATH = os.getenv('RHF_TOOL_CHAINS')
else:
    TOOL_PATH = 'C:/gcc-arm-none-eabi/bin'
    print('Error macro RHF_TOOL_CHAINS not find!!!')

print('TOOL_PATH: ', TOOL_PATH)
TOOL_INCPATH = os.path.join(TOOL_PATH, '../arm-none-eabi/include')
TOOL_LIBPATH = os.path.join(TOOL_PATH, '../arm-none-eabi/lib')

BULID_LIST_ROOT_PATH = BULID_OBJ_ROOT_PATH = os.path.join('bulid', SCON_PROJECT_NAME, 'list')

if not os.path.exists(BULID_LIST_ROOT_PATH):
    os.makedirs(BULID_LIST_ROOT_PATH)
BULID_LIST_MAP_BASE_NAME = os.path.join(BULID_LIST_ROOT_PATH, SCON_PROJECT_NAME)

PREFIX = 'arm-none-eabi-'
CC = PREFIX + 'gcc'
AS = PREFIX + 'gcc'
AR = PREFIX + 'ar'
CXX = PREFIX + 'g++'
LINK = PREFIX + 'gcc'
TARGET_EXT = 'elf'
SIZE = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY = PREFIX + 'objcopy'

if SCON_PROJECT_NAME.upper() in module_list_mcu_wlxx:
    MCU = 'cortex-m4'
elif SCON_PROJECT_NAME.upper() in module_list_mcu_l0xx:
    MCU = 'cortex-m0plus'
else:
    print('Not find MCU')
    exit()

DEVICE = f' -mcpu={MCU} -mthumb -ffunction-sections -fdata-sections'
CFLAGS = DEVICE + ' -Dgcc -Wall'
AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp -Wa,-mimplicit-it=thumb '
# LFLAGS = DEVICE + ' -Wl,--gc-sections,-Map={}.map,--cref,-u,Reset_Handler -T{} -specs=nano.specs -specs=nosys.specs'.format(SCON_PROJECT_NAME, LINKER_SCRIPT)
LFLAGS = DEVICE + ' -Wl,--gc-sections,-Map={}.map,--cref -T{} -specs=nano.specs -specs=nosys.specs -lc -lm'.format(BULID_LIST_MAP_BASE_NAME, LINKER_SCRIPT)

if BUILD == 'Debug':
    CFLAGS += ' -O0 -gdwarf-2 -g'
    AFLAGS += ' -gdwarf-2'
else:
    CFLAGS += ' -Os'

CXXFLAGS = CFLAGS 

SCONS_TARGET = '{}.{}'.format(SCON_PROJECT_NAME, TARGET_EXT)
SCONS_TARGET_HEX = '{}.hex'.format(SCON_PROJECT_NAME)
SCONS_TARGET_BIN = '{}.bin'.format(SCON_PROJECT_NAME)

# DefaultEnvironment(tools=[])
env = Environment(TOOLS = ['mingw'],
    AS = AS, ASFLAGS = AFLAGS,
    CC = CC, CCFLAGS = CFLAGS,
    AR = AR, ARFLAGS = '-rc',
    CXX = CXX, CXXFLAGS = CXXFLAGS,
    LINK = LINK, LINKFLAGS = LFLAGS)
env.PrependENVPath('PATH', TOOL_PATH)
    
env.Replace(
    ARCOMSTR = 'AR $TARGET',
    ASCOMSTR = 'AS $TARGET',
    ASPPCOMSTR = 'AS $TARGET',
    CCCOMSTR = 'CC $TARGET',
    CXXCOMSTR = 'CXX $TARGET',
    LINKCOMSTR = 'LINK $TARGET'
)

# MACRO config
CPPDEFINES = [
    'USE_HAL_DRIVER',
    'USE_FULL_ASSERT',
    'SOFT_SE',
]
if SCON_PROJECT_NAME.upper() in module_list_use_tcxo:
    CPPDEFINES.append('USE_TCXO')

if SCON_PROJECT_NAME.upper() in module_list_mcu_wlxx:
    CPPDEFINES.append('CORE_CM4')
    CPPDEFINES.append('STM32WLE5xx')
elif SCON_PROJECT_NAME.upper() in module_list_mcu_l0xx:
    CPPDEFINES.append('CORE_CM0')
    CPPDEFINES.append('STM32L071xx')
else:
    print('Not find MCU')
    exit()

# add region band macro
for band in region_band_list:
    CPPDEFINES.append(band)

# for mac in CPPDEFINES:
    # print(mac)
env.AppendUnique(CPPDEFINES = CPPDEFINES)
# env.AppendUnique(CPPPATH = TOOL_INCPATH)
# env.AppendUnique(LIBPATH = TOOL_LIBPATH)

SOURCE = []
ASOURCE = []

BULID_OBJ_ROOT_PATH = os.path.join('bulid', SCON_PROJECT_NAME, 'obj')
BULID_OBJ_BOARD_PATH = os.path.join(BULID_OBJ_ROOT_PATH, 'bsp/board')
BULID_OBJ_STM32_HAL_PATH = os.path.join(BULID_OBJ_ROOT_PATH, 'bsp/stm32hal')
BULID_OBJ_SYSTEM_PATH = os.path.join(BULID_OBJ_ROOT_PATH, 'system')
BULID_OBJ_RADIO_PATH = os.path.join(BULID_OBJ_ROOT_PATH, 'radio')
BULID_OBJ_APP_PATH = os.path.join(BULID_OBJ_ROOT_PATH, 'app')
BULID_OBJ_MAC_PATH = os.path.join(BULID_OBJ_ROOT_PATH, 'mac')

BULID_TARGET_ROOT_PATH = os.path.join('bulid', SCON_PROJECT_NAME, 'exe')
SCONS_TARGET = os.path.join(BULID_TARGET_ROOT_PATH, SCONS_TARGET)
SCONS_TARGET_HEX = os.path.join(BULID_TARGET_ROOT_PATH, SCONS_TARGET_HEX)
SCONS_TARGET_BIN = os.path.join(BULID_TARGET_ROOT_PATH, SCONS_TARGET_BIN)

POST_ACTION  = OBJCPY + ' -O ihex $TARGET {}\n'.format(SCONS_TARGET_HEX)
POST_ACTION += OBJCPY + ' -O binary $TARGET {}\n'.format(SCONS_TARGET_BIN)
POST_ACTION += (SIZE + ' $TARGET \n')

group = SConscript(f'src/boards/{SCON_PROJECT_NAME.upper()}/SConscript', variant_dir=BULID_OBJ_BOARD_PATH, duplicate=False)
env.AppendUnique(CPPPATH = group['CPPPATH'])
env.AppendUnique(CPPDEFINES = group['CPPDEFINES'])
SOURCE += group['SOURCE']

if SCON_PROJECT_NAME.upper() in module_list_mcu_wlxx:
    group = SConscript('src/boards/mcu/stm32wl/SConscript', variant_dir=BULID_OBJ_STM32_HAL_PATH, duplicate=False)
elif SCON_PROJECT_NAME.upper() in module_list_mcu_l0xx:
    group = SConscript('src/boards/mcu/stm32l0/SConscript', variant_dir=BULID_OBJ_STM32_HAL_PATH, duplicate=False)
else:
    print('Not find MCU')
    exit()
env.AppendUnique(CPPPATH = group['CPPPATH'])
env.AppendUnique(CPPDEFINES = group['CPPDEFINES'])
SOURCE += group['SOURCE']

group = SConscript('src/system/SConscript', variant_dir=BULID_OBJ_SYSTEM_PATH, duplicate=False)
env.AppendUnique(CPPPATH = group['CPPPATH'])
env.AppendUnique(CPPDEFINES = group['CPPDEFINES'])
SOURCE += group['SOURCE']

group = SConscript('src/radio/SConscript', variant_dir=BULID_OBJ_RADIO_PATH, duplicate=False)
env.AppendUnique(CPPPATH = group['CPPPATH'])
env.AppendUnique(CPPDEFINES = group['CPPDEFINES'])
SOURCE += group['SOURCE']

group = SConscript('src/mac/SConscript', variant_dir=BULID_OBJ_MAC_PATH, duplicate=False)
env.AppendUnique(CPPPATH = group['CPPPATH'])
env.AppendUnique(CPPDEFINES = group['CPPDEFINES'])
SOURCE += group['SOURCE']

group = SConscript('src/apps/LoRaMac/SConscript', variant_dir=BULID_OBJ_APP_PATH, duplicate=False)
env.AppendUnique(CPPPATH = group['CPPPATH'])
env.AppendUnique(CPPDEFINES = group['CPPDEFINES'])
SOURCE += group['SOURCE']

# print(env.Dump())
# print('------------------')
# for index in SOURCE:
#     print(index)
# print('------------------')
# for index in env['CPPPATH']:
#     print(index)
# print('------------------')
# print(len(SOURCE))

env.Program(SCONS_TARGET, SOURCE)

env.AddPostAction(SCONS_TARGET, POST_ACTION)
