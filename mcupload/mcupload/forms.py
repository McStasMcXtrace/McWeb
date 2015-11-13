from django import forms
from sim_file_handler import group_dirs

groups = (('1', 'one'), ('2', 'two'))

class SimFileForm(forms.Form):
    username = forms.CharField(max_length=32)
    password = forms.CharField(max_length=32)
    sim_file = forms.FileField()
    group    = forms.ChoiceField(choices=group_dirs(), required=True) # use to get dir to put sim_file in

    print group_dirs()
