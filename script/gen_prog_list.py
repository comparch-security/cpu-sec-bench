import os
import time
import subprocess
import xml.etree.ElementTree as ET

"this script is used to generatesafety-elatedd options in Windows exploit protection. And it can be extended to conduct configuration whichorganizesd various compilers' safety options."

class safety_feature:
    def __init__(self, name, attrs, target):
        self.name = name
        self.attrs = attrs
        self.target = target

directory = './test'  # directory which sotres binary executation
windows_exploit_protection_feature = [
    safety_feature("DEP", {"Enable":"true","EmulateAtlThunks":"false"}, "windows-exploit"),
    safety_feature("ASLR", {"ForceRelocateImages":"false","RequireInfo":"false","BottomUp":"true","HighEntropy":"true"}, "windows-exploit"),
    safety_feature("StrictHandle", {"Enable":"false"}, "windows-exploit"),
    safety_feature("ExtensionPoints", {"DisableExtensionPoints":"false"}, "windows-exploit"),
    safety_feature("DynamicCode", {"BlockDynamicCode":"false","AllowThreadsToOptOut":"false"}, "windows-exploit"),
    safety_feature("ControlFlowGuard", {"Enable":"true","SuppressExports":"false"}, "windows-exploit"),
    safety_feature("Payload", {"EnableExportAddressFilter":"false","EnableImportAddressFilter":"false", "EnableRopStackPivot":"false", "EnableRopCallerCheck":"false", "EnableRopSimExec":"false"}, "windows-exploit"),
    safety_feature("SEHOP", {"Enable":"true","TelemetryOnly":"false"}, "windows-exploit"),
    safety_feature("Heap", {"TerminateOnError":"true"}, "windows-exploit"),
    safety_feature("UserShadowStack", {"UserShadowStack":"false", "UserShadowStackStrictMode":"false"}, "windows-exploit"),
]

def indent(elem, level=1):
    i = "\n" + level*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        for elem in elem:
            indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

def get_exe_files(directory):
    exe_files = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".exe"):
                exe_files.append(file)
    return exe_files

def run_powershell_command_as_admin(cmd):
    # Run the PowerShell command as admin
    ps_command = f'powershell.exe Start-Process -FilePath powershell.exe -ArgumentList \'{cmd}\' -Verb RunAs'
    # run will auto wait, the orig func Popen not
    ret = subprocess.run(ps_command, shell=True)
    return ret

def import_windows_exploit_protection_xml(cmd, file_path, timeout=60):

    run_powershell_command_as_admin(cmd)

    # Wait for the file to be created by the PowerShell command
    start_time = time.time()
    while not os.path.exists(file_path):
        if time.time() - start_time > timeout:
            raise TimeoutError(f'Timeout after {timeout} seconds waiting for file {file_path} to be created.')
        time.sleep(1)  # wait for 1 second before checking again

    # Read the file
    try:
        with open(file_path, 'r') as file:
            content = file.read()
        return content
    except Exception as e:
        print(f'Error reading file {file_path}: {str(e)}')

def export_windows_exploit_protection_xml(cmd):
    run_powershell_command_as_admin(cmd)

# The written XML element format is:
# <Record>
#     <Field1>New field value</Field1>
#                   ...
#     <FieldN>New field value</FieldN>
# </Record>
def add_new_record_and_insert_in_xml(tree,recordf,attrs,text):

    # Parse the XML file
    root = tree.getroot()

    # Create a new record
    new_record = ET.Element(recordf)
    for (key,val) in attrs.items():
        print("key is {}, value is {}".format(key,val))
        new_record.attrib[key] = val
    new_record.text = text
    root.insert(0,new_record)
    return new_record

def add_new_subelement(element,recordf,attrs,text):
    new_sub_record = ET.SubElement(element, recordf)
    for (key,val) in attrs.items():
        new_sub_record.attrib[key] = val
    new_sub_record.text = text

def generate_exploit_protection_config(exes):
    the_original_path = os.path.abspath('./windows_ep_original.xml')
    the_modified_path = os.path.abspath('./windows_ep_modified.xml')
    the_orig_xml = import_windows_exploit_protection_xml(
        "Get-ProcessMitigation -RegistryConfigFilePath {}".format(the_original_path),
        the_original_path)
    #print(the_orig_xml)
    print("the original path is {}".format(the_original_path))
    tree = ET.parse(the_original_path)
    for exe in exe_files:
        curr_node = add_new_record_and_insert_in_xml(tree,"AppConfig", {"Executable":"{}".format(exe)},"")
        for s in windows_exploit_protection_feature:
            add_new_subelement(curr_node, s.name, s.attrs ,"")
        indent(curr_node)
    # Write the modified XML data to a new file
    tree.write(the_modified_path)
    #export_windows_exploit_protection_xml("Set-ProcessMitigation -PolicyFilePath {}".format(the_modified_path))


# Use the function
exe_files = get_exe_files(directory)
# print(exe_files)
# print(len(exe_files))
generate_exploit_protection_config(exe_files)