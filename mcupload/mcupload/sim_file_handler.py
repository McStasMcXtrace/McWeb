from os import walk
from settings import BASE_DIR
from re import split

# drop down menu choices
def group_dirs():
    group_tups = ()
    current_sim_dir = '../mcweb/sim'
    for group_dir_path, _, filenames in walk(current_sim_dir):
        for f in filenames:
            if f.endswith('.instr'):
                group_tups =group_tups + ((group_dir_path.replace('../mcweb/sim/', ''), group_dir_path),)
                break
    return group_tups

def finger_group(group_selection):
    for line in group_selection:
        if 'selected' in line:

            print split("\>", line)


# actually handling the instr file
def shepherd(sim_file, group_selection):
    group_dir = finger_group(str(group_selection))
    print group_dir
    if not sim_file.name.endswith('.instr'):
        file_path = BASE_DIR+"not_instr/"+sim_file.name
    else:
        file_path = "%s/%s"%(group_dir,sim_file.name)
    with open(file_path, 'wb+') as out_file:
        for chunk in sim_file.chunks():
            out_file.write(chunk)
    return "saved to: %s"%file_path
