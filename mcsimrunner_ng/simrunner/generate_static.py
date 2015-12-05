#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Generates static html browser pages for simrunner data output.
Uses django templates to generate the html.
'''
from django.template import Context
from os.path import basename, join, splitext
from django.template.loader import get_template

class McStaticDataBrowserGenerator():
    ''' implements functionality to generate static html site for data browsing '''    
    def generate_browsepage(self, data_folder, png_files, dat_files):
        ''' png_files AND dat_files must be of the correct path relative to data_folder 
            note: having dat_files be relative paths reduces assumptions and increases flexibility
        '''
        # get data for browse.html
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
        html_paths = map(lambda p: join(data_folder, '%s%s' % (splitext(p)[0], '.html')), png_files)
        
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
        
        f = open(join(data_folder, 'browse.html'), 'w')
        f.write(html)
        f.close()
        
    def generate_browsepage_sweep(self, data_folder, png_files, dat_files, scanpoints):
        ''' as above, but handles the simulation scan case '''
        # prepare strings
        png_base = map(lambda p: basename(p), png_files)
        dat_base = map(lambda d: basename(d), dat_files)
        html_paths = map(lambda p: join(data_folder, '%s%s' % (splitext(p)[0], '.html')), png_files)
        
        # 1) write <monitor>.html:
        
        # write <monitor>.html for each png/dat file
        t = get_template('static_monitor.html')
        for j in range(scanpoints):
            for i in range(len(png_files)):
                if i == 0:
                    continue
                
                png = png_base[i].replace('/0/', '/%s/' % str(j))
                dat = dat_base[i].replace('/0/', '/%s/' % str(j))
                
                png_dat = [png, dat]
                c = Context({'png_dat': png_dat})
                html = t.render(c)
                
                html_path = html_paths[i].replace('/0/', '/%s/' % str(j))
                f = open(html_path, 'w')
                f.write(html)
                f.close()
        
        # special case: mccode.dat : sweep overview
        png_dat = [png_base[0], dat_base[0]]
        c = Context({'png_dat': png_dat})
        html = t.render(c)
        
        f = open(html_paths[0], 'w')
        f.write(html)
        f.close()
        
        # 2 write <monitor>_ss.html
        
        # get data for each monitor_ss.html and write it immediately
        t = get_template('monitor_ss.html')
        for i in range(len(png_files)):
            if i == 0:
                continue
            
            html_png_dat = []
            monitor_name = splitext(basename(png_files[i]))[0]
            for j in range(scanpoints):
                png = png_files[i].replace('/0/', '/%s/' % str(j))
                
                dat = dat_files[i].replace('/0/', '/%s/' % str(j))
                
                html = splitext(png_files[i])[0] + '.html'
                html = html.replace('/0/', '/%s/' % str(j))
                
                html_png_dat.append([html, png, dat])
                
            c = Context({'legend': monitor_name, 'html_png_dat': html_png_dat})
            html = t.render(c)
            
            f = open(join(data_folder, '%s_sweep.html' % monitor_name), 'w')
            f.write(html)
            f.close()
        
        # 3) write browse.html
        
        # get data for browse_ss.html
        # the list html_name contains list of monitors (monitor.0 is html filepath,  monitor.1 is monitor displayname)
        sim_html = html_paths[0]
        sim_png = png_files[0]
        html_name = []
        for i in range(len(png_files)):
            if i == 0:
                continue # avoid index 0 at all cost for sweeps!
            
            monitor_name = splitext(basename(png_files[i]))[0]
            html_filepath = join(data_folder, '%s_sweep.html' % monitor_name)
            html_name.append([html_filepath, monitor_name]) 
        
        t = get_template('static_browse_ss.html')
        c = Context({'sim_html': sim_html, 'sim_png': sim_png, 'html_name': html_name})
        html = t.render(c)
        
        f = open(join(data_folder, 'browse.html'), 'w')
        f.write(html)
        f.close()

