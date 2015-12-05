#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Generates static html browser pages for simrunner data output.
Uses django templates to generate the html.
'''
import logging
import argparse
from django.template import Template, Context
from os.path import basename, join, splitext
from django.template.loader import get_template

class McStaticDataBrowserGenerator():
    ''' implements functionality to generate static html site for data browsing '''    
    __basedir = ''
    def __init__(self, basedir=''):
        self.__basedir = basedir
    
    def generate_browsepage(self, data_folder, png_files, dat_files, templ_getfunc = get_template):
        ''' png_files AND dat_files must be of the correct path relative to data_folder 
            note: having dat_files be relative paths reduces assumptions and increases flexibility
        '''
        # write data for browse.html
        html_png_dat = []
        for i in range(len(png_files)):
            png = png_files[i]
            dat = dat_files[i]
            html = splitext(png_files[i])[0] + '.html'
            html_png_dat.append([html, png, dat])
        
        # 1) write <monitor>.html:
        
        # prepare strings
        png_base = map(lambda p: basename(p), png_files)
        dat_base = map(lambda d: basename(d), dat_files)
        html_paths = map(lambda p: join(self.__basedir, data_folder, '%s%s' % (splitext(p)[0], '.html')), png_files)
        
        # get template for <monitor>.html
        t = get_template('static_monitor.html')

        # write <monitor>.html for each png/dat file
        for i in range(len(png_files)):
            png_dat = [png_base[i], dat_base[i]]
            c = Context({'png_dat': png_dat})
            html = t.render(c)
            
            f = open(html_paths[i], 'w')
            f.write(html)
            f.close()
        
        # 2) write browse.html
        
        t = get_template('static_browse.html')
        c = Context({'html_png_dat': html_png_dat})
        html = t.render(c)
        
        f = open(join(self.__basedir, data_folder, 'browse.html'), 'w')
        f.write(html)
        f.close()
        
    def generate_browsepage_sweep(self, folder):
        ''' as above, but handles the simulation scan case '''
        pass

def get_simrunner_template(basedir, template_name):
    t_text = open('%s/simrunner/templates/%s' % (basedir, template_name)).read()
    t = Template(t_text)
    return t

def main(args):
    logging.basicConfig(level=logging.INFO)
    
    folder = 'static/data/hest_PSI_DMC_20151205_110959'
    png_files = ['mcstas/detector.png', 'mcstas/L_in.png', 'mcstas/PSD_sample.png']
    dat_files = ['mcstas/detector.dat', 'mcstas/L_in.dat', 'mcstas/PSD_sample.dat']
    
    generator = McStaticDataBrowserGenerator('/home/jaga/simrunner')
    generator.generate_browsepage(folder, png_files, dat_files)
    
if __name__ == '__main__':
    # enable django template functionality without importing everything
    from django.conf import settings
    settings.configure()
    
    parser = argparse.ArgumentParser(description=__doc__)
    #parser.add_argument('comproot', nargs='+', help='mcstas-comps directory searched recursively.')
    #parser.add_argument('ofile', help='Output file name')
    args = parser.parse_args()

    main(args)
