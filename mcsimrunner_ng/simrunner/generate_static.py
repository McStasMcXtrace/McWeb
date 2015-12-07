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
     
    __base_c_dict = None
    def set_base_context(self, dict):
        ''' sets the dict that is passed to every context '''
        self.__base_c_dict = dict
    def get_context(self, dict):
        ''' combile "local" context with base context set at the beginning '''
        d = self.__base_c_dict.copy()
        d.update(dict)
        return Context(d) 
    
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
            
        # create twin data for log
        html_png_dat_log = []
        for i in range(len(png_files)):
            png = splitext(png_files[i])[0] + '_log.png'
            dat = dat_files[i]
            html = splitext(png_files[i])[0] + '_log.html'
            html_png_dat_log.append([html, png, dat])
        
        # 1) write <monitor>.html:
        
        # prepare strings
        png_base = map(lambda p: basename(p), png_files)
        dat_base = map(lambda d: basename(d), dat_files)
        html_paths = map(lambda p: join(data_folder, '%s%s' % (splitext(p)[0], '.html')), png_files)
        
        # get template for <monitor>.html
        t = get_template('static_monitor.html')

        # write <monitor>.html and <monitor>_log.html for each png/dat file
        for i in range(len(png_files)):
            png_dat = [png_base[i], dat_base[i]]
            
            c = Context({'png_dat': png_dat})
            write_html(html_paths[i], t.render(c))
            
            # log versions of monitor files
            png_dat_log = [splitext(png_base[i])[0] + '_log.png', dat_base[i]]
            
            c = Context({'png_dat': png_dat_log})
            write_html(splitext(html_paths[i])[0] + '_log.html', t.render(c))
        
        # 2) write browse.html
        
        t = get_template('static_browse.html')
        c = self.get_context({'html_png_dat': html_png_dat, 'twin_html': 'browse_log.html', 'lin_or_log': 'log'})
        write_html(join(data_folder, 'browse.html'), t.render(c))
        
        c = self.get_context({'html_png_dat': html_png_dat_log, 'twin_html': 'browse.html', 'lin_or_log': 'lin'})
        write_html(join(data_folder, 'browse_log.html'), t.render(c))
        
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
                write_html(html_paths[i].replace('/0/', '/%s/' % str(j)), t.render(c))
                
        # special case: mccode.dat : sweep overview
        png_dat = [png_base[0], dat_base[0]]
        c = Context({'png_dat': png_dat})
        write_html(html_paths[0], t.render(c))
        
        # 2 write <monitor>_ss.html
        
        # get data for each monitor_ss.html and write it immediately
        t = get_template('static_monitor_sweep.html')
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
                
            c = self.get_context({'monitor_name': monitor_name, 'html_png_dat': html_png_dat})
            write_html(join(data_folder, '%s_sweep.html' % monitor_name), t.render(c))
            
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
        
        # write browse.html
        t = get_template('static_browse_sweep.html')
        c = self.get_context({'sim_html': sim_html, 'sim_png': sim_png, 'html_name': html_name})
        write_html(join(data_folder, 'browse.html'), t.render(c))

def write_html(filepath, text):
    ''' writes file <filepath> with content <text> to disk '''
    f = open(filepath, 'w')
    f.write(text)
    f.close()
