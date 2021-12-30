__hh__ = '''
/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
 '''

import sys
import os
from PIL import Image
import xml.etree.ElementTree as ET
import re
import glob
import pickle
import hashlib

reSgBMP = re.compile(r'SgBMP\{(\d+)\}')
reSgTXT = re.compile(r'SgTXT\{(\d+)\}')

__SGL_MAX = 0


def CName(s):
    return s.replace('.', '_').replace('/', '_').replace('\\', '_')


class Sg():
    def __init__(self, file, option=None):
        self.file = file
        self.option = option

    def getPixel(self, IM, x, y):
        rgb = IM.getpixel((x, y))
        if(type(rgb) == int):
            pass
        elif(len(rgb) == 4):
            rgb = (rgb[0] << 16) + (rgb[1] << 8) + rgb[2] + (rgb[3] << 24)
        elif(len(rgb) == 3):
            rgb = (rgb[0] << 16) + (rgb[1] << 8) + rgb[2]
        else:
            print('rgb is', rgb)
            assert(0)
        return rgb

    def toU8Dot(self, fp, X=0, Y=0):

        name = os.path.basename(self.file)
        name = name[:name.find('.')]
        code = hex(ord(name))[2:].upper()
        aname = os.path.abspath(self.file)
        fp.write('static const uint8_t sgf_dot_%s[] = \n{' % (code))

        try:
            IM = Image.open(self.file)
        except NameError:
            fp.write('  0\n};\n')
            return
        fp.write('\n  %s,%s,/* size(w,h) */' % (IM.size[0], IM.size[1]))
        for y in range(0, IM.size[1]):
            fp.write('\n  ')
            DOT = B = 0
            for x in range(0, IM.size[0]):
                rgb = self.getPixel(IM, x, y)
                if(rgb != 0):
                    DOT = DOT | (1 << B)
                B += 1
                if(B == 8):
                    fp.write('0x%-2X,' % (DOT))
                    DOT = 0
                    B = 0
            if(B > 0):
                fp.write('0x%-2X,' % (DOT))
        fp.write('\n};\n')

    def filterForTelltale(self, rgb):
        # as I don't want to use GIMP to create each valid TT PNG, so just
        # use this API to filter out the black color
        alpha = (rgb >> 24) & 0xFF
        # if(alpha == 0): # this is empty color
        #    return 0
        r = (rgb >> 16) & 0xFF
        g = (rgb >> 8) & 0xFF
        b = (rgb >> 0) & 0xFF
        if(r < 80 and g < 80 and b < 80):  # black
            return 0
        if(r > 0xE0 and g > 0xE0 and b > 0xE0):  # white
            return 0
        return rgb

    def toU8Pixel(self, fp, X=0, Y=0):
        name = CName(os.path.basename(self.file))
        aname = os.path.abspath(self.file)
        fp.write('#include "Sg.h"\n')
        fp.write('#include "SgRes.h"\n\n')
        fp.write('static const uint32_t %s_bmp[] = \n{' % (name))
        IM = Image.open(self.file)
        for y in range(0, IM.size[1]):  # height
            fp.write('\n  ')
            for x in range(0, IM.size[0]):  # width
                rgb = self.getPixel(IM, x, y)
                if(name[:3] == 'tt_'):
                    rgb = self.filterForTelltale(rgb)
                fp.write('0x%-8X,' % (rgb))
        fp.write('\n};\n')
        fp.write('static const SgBMP %s_BMP=\n' % (name))
        fp.write('{  /* %s */\n' % (aname))
        fp.write('  /*x=*/%s,\n' % (X))
        fp.write('  /*y=*/%s,\n' % (Y))
        fp.write('  /*w=*/%s,\n' % (IM.size[0]))  # width
        fp.write('  /*h=*/%s,\n' % (IM.size[1]))  # height
        fp.write('  /*p=*/%s_bmp\n};\n' % (name))


def GetSgImage(IML, fp):
    d = os.path.abspath(os.path.dirname(fp.name) + '/..')
    for image in IML:
        Sg('%s/%s' % (d, image[0])).toU8Pixel(fp, image[1], image[2])


def GetSgFont(IML, fp):
    d = os.path.abspath(os.path.dirname(fp.name) + '/..')
    for image in IML:
        Sg('%s/%s' % (d, image)).toU8Dot(fp)


def GenearteSgBMP(widget, fph, fpc):
    global __SGL_MAX
    d = os.path.dirname(fph.name)
    fp = open('%s/%s.c' % (d, widget.attrib['name']), 'w')
    size = int(reSgBMP.search(widget.attrib['type']).groups()[0], 10)
    IML = []
    for p in widget:
        if(p.tag == 'SgBMP'):
            IML.append((p.attrib['locate'], p.attrib['x'], p.attrib['y']))
    if(len(IML) == size):
        GetSgImage(IML, fp)
    else:
        raise Exception(
            'size SG widget picture is not right <size=%s,len(SgPciture)=%s>!' % (size, len(IML)))
    fp.write(
        'static const SgBMP* %s_BMPS[%s] = \n{\n' % (widget.attrib['name'], size))
    for i, (file, x, y) in enumerate(IML):
        name = CName(os.path.basename(file))
        fp.write('  &%s_BMP,\n' % (name))
        fph.write("#define SGR_%-32s %s\n" % (name.upper(), i))
    fp.write('};\n\n')

    fp.write('const SgSRC %s_SRC = \n{\n' % (widget.attrib['name']))
    fp.write('  /*t =*/%s,\n' % ('SGT_BMP'))
    fp.write('  /*rs=*/%s,\n' % (size))
    fp.write('  /*r =*/(const SgRes**)%s_BMPS,\n' % (widget.attrib['name']))
    fp.write('  /*rf=*/(void*(*)(void*))%s,\n' % (widget.attrib['refresh']))
    fp.write('  /*cf=*/(void(*)(void*))%s,\n' % (widget.attrib['cache']))
    fp.write('  /*weight=*/%s,\n' % (widget.attrib['weight']))
    fp.write('  /*name=*/"%s"\n' % (widget.attrib['name']))
    fp.write('};\n\n')

    if(widget.attrib['refresh'] != 'NULL'):
        fph.write('extern void* %s(SgWidget* w);\n' %
                  (widget.attrib['refresh']))

    if(widget.attrib['cache'] != 'NULL'):
        fph.write('extern void %s(SgWidget* w);\n' % (widget.attrib['cache']))

    fph.write('extern const SgSRC %s_SRC;\n' % (widget.attrib['name']))

    fpc.write('  { /* SGW_%s */\n' % (widget.attrib['name'].upper()))
    fpc.write('    /*x =*/%s,\n' % (widget.attrib['x']))
    fpc.write('    /*y =*/%s,\n' % (widget.attrib['y']))
    fpc.write('    /*w =*/%s,\n' % (widget.attrib['w']))
    fpc.write('    /*h =*/%s,\n' % (widget.attrib['h']))
    fpc.write('    /*c =*/%s,\n' % (0))
    fpc.write('    /*d =*/%s,\n' % ('0xFFFF/*no rotation*/'))
    fpc.write('    /*l =*/%s,\n' % (widget.attrib['layer']))
    if(int(widget.attrib['layer'], 10) > __SGL_MAX):
        __SGL_MAX = int(widget.attrib['layer'], 10)
    name = CName(os.path.basename(IML[0][0]))
    fpc.write('    /*ri=*/SGR_%s,\n' % (name.upper()))
    fpc.write('    /*src =*/&%s_SRC\n' % (widget.attrib['name']))
    fpc.write('  },\n')
    fp.close()


def GenearteSgTXT(widget, fph, fpc):
    global __SGL_MAX
    d = os.path.dirname(fph.name)
    fp = open('%s/%s.c' % (d, widget.attrib['name']), 'w')
    fp.write('#include "Sg.h"\n')
    fp.write('#include "SgRes.h"\n\n')
    size = int(reSgTXT.search(widget.attrib['type']).groups()[0], 10)

    FNT = []
    for p in widget:
        if(p.tag == 'SgTXT'):
            FNT.append(p.attrib['refer'])
    if(len(FNT) == size):
        pass
    else:
        raise Exception(
            'size SG widget text is not right <size=%s,len(SgTXT)=%s>!' % (size, len(FNT)))
    fp.write(
        'static const SgTXT* %s_TXTS[%s] = \n{\n' % (widget.attrib['name'], size))
    for i, font_name in enumerate(FNT):
        fp.write('  &sgf%s,\n' % (font_name))
        fph.write("#define SGR_%-32s %s\n" % (font_name.upper(), i))
    fp.write('};\n\n')

    if(widget.attrib['refresh'] != 'NULL'):
        fph.write('extern void* %s(SgWidget* w);\n' %
                  (widget.attrib['refresh']))

    fph.write('extern const SgSRC %s_SRC;\n' % (widget.attrib['name']))
    fp.write('const SgSRC %s_SRC = \n{\n' % (widget.attrib['name']))
    fp.write('  /*t =*/%s,\n' % ('SGT_TXT'))
    fp.write('  /*rs=*/%s,\n' % (size))
    fp.write('  /*r =*/(const SgRes**)%s_TXTS,\n' % (widget.attrib['name']))
    fp.write('  /*rf=*/(void*(*)(void*))%s\n' % (widget.attrib['refresh']))
    fp.write('};\n\n')

    fpc.write('  { /* SGW_%s */\n' % (widget.attrib['name'].upper()))
    fpc.write('    /*x =*/%s,\n' % (widget.attrib['x']))
    fpc.write('    /*y =*/%s,\n' % (widget.attrib['y']))
    fpc.write('    /*w =*/%s,\n' % (widget.attrib['w']))
    fpc.write('    /*h =*/%s,\n' % (widget.attrib['h']))
    fpc.write('    /*c =*/%s,\n' % (0))
    fpc.write('    /*d =*/%s,\n' % ('0xFFFF/*no rotation*/'))
    fpc.write('    /*l =*/%s,\n' % (widget.attrib['layer']))
    if(int(widget.attrib['layer'], 10) > __SGL_MAX):
        __SGL_MAX = int(widget.attrib['layer'], 10)
    fpc.write('    /*ri=*/SGR_%s,\n' % (font_name.upper()))
    fpc.write('    /*src =*/&%s_SRC\n' % (widget.attrib['name']))
    fpc.write('  },\n')

    fp.close()


def GenearteSgFont(widget, fph, fpc):
    locate = widget.attrib['locate']
    font_name = widget.attrib['name']
    d = os.path.dirname(fph.name)
    fp = open('%s/%s.c' % (d, font_name), 'w')
    fp.write('#include "Sg.h"\n')
    fp.write('#include "SgRes.h"\n\n')
    IML = []
    for png in glob.glob('%s/*.png' % (locate)):
        IML.append(png)
    GetSgFont(IML, fp)

    fp.write('static const uint16_t sgf_%s_look_up_table[%s] = \n{' % (
        font_name, len(IML)))
    chars = []
    for png in IML:
        name = os.path.basename(png)
        name = name[:name.find('.')]
        code = ord(name)
        chars.append(code)
    chars.sort()
    for i, chr in enumerate(chars):
        if(i % 4 == 0):
            fp.write('\n  ')
        fp.write('0x%-4s,' % (hex(chr).upper()[2:]))
    fp.write('\n};\n\n')

    fp.write(
        'static const uint8_t* sgf_%s_res_pointer_table[%s] = \n{' % (font_name, len(IML)))
    for i, chr in enumerate(chars):
        if(i % 4 == 0):
            fp.write('\n  ')
        fp.write('sgf_dot_%-4s,' % (hex(chr).upper()[2:]))
    fp.write('\n};\n\n')

    fph.write('extern const SgTXT sgf%s;\n' % (font_name))

    fp.write('const SgTXT sgf%s = \n{\n' % (font_name))
    fp.write('  /*l=*/sgf_%s_look_up_table,\n' % (font_name))
    fp.write('  /*p=*/sgf_%s_res_pointer_table,\n' % (font_name))
    fp.write('  /*w=*/%s,\n' % (widget.attrib['w']))
    fp.write('  /*h=*/%s,\n' % (widget.attrib['h']))
    fp.write('  /*s=*/%s\n' % (len(IML)))
    fp.write('};\n\n')

    fp.close()


def GenerateWidget(widget, fph, fpc):
    print('## Process Widget %s' % (widget.attrib['name']))

    if(reSgBMP.search(widget.attrib['type']) != None):
        GenearteSgBMP(widget, fph, fpc)
    elif(reSgTXT.search(widget.attrib['type']) != None):
        GenearteSgTXT(widget, fph, fpc)
    elif(widget.attrib['type'] == 'SgFont'):
        GenearteSgFont(widget, fph, fpc)
    else:
        raise Exception('unknown SG widget type!')


def GenerateSg(file):
    global __SGL_MAX
    d = os.path.dirname(file)
    pklpath = '%s/SgRes/.gen.pkl' % (d)
    md5 = hashlib.md5()
    md5.update(open(file, 'rb').read())
    new_hash = md5.hexdigest()
    if(os.path.exists('%s/SgRes' % (d)) == False):
        os.mkdir('%s/SgRes' % (d))
    else:
        if os.path.exists(pklpath):
            old_hash = pickle.load(open(pklpath, 'rb'))
            if new_hash == old_hash:
                return
    root = ET.parse(file).getroot()
    widgets = []
    for w in root:
        if(w.tag == 'SgWidget'):
            widgets.append(w)

    fonts = []
    for f in root:
        if(f.tag == 'SgFont'):
            fonts.append(f)

    fph = open('%s/SgRes/SgRes.h' % (d), 'w')
    fpc = open('%s/SgRes/SgRes.c' % (d), 'w')
    fpc.write(__hh__)
    fpc.write('#include "Sg.h"\n')
    fpc.write('#include "SgRes.h"\n\n')
    fph.write(__hh__)
    fph.write('\n#ifndef SGRES_H\n#define SGRES_H\n\n')
    fph.write('#define __SG_WIDTH__ %s\n' % (root.attrib['w']))
    fph.write('#define __SG_HEIGHT__ %s\n\n' % (root.attrib['h']))
    fph.write('#define __SG_PIXEL__ %s\n\n' % (root.attrib['p']))

    fpc.write('SgWidget SGWidget[%s] = \n{\n' % (len(widgets)))
    for w in widgets:
        GenerateWidget(w, fph, fpc)
    fpc.write('};\n\n')
    # font is a special widget
    for f in fonts:
        GenerateWidget(f, fph, fpc)

    fph.write('\n\n')
    for i, w in enumerate(widgets):
        fph.write('#define SGW_%-32s %s\n' % (w.attrib['name'].upper(), i))
    fph.write('#define SGW_%-32s %s\n' % ('MAX', len(widgets)))
    fph.write('\n\nextern SgWidget SGWidget[%s];\n\n' % (len(widgets)))
    fph.write("\n\n#define SGL_MAX %s\n\n" % (__SGL_MAX+1))
    fph.write('#endif\n\n')
    fph.close()
    fpc.close()
    print(">>>>>>>> DONE! <<<<<<<<<")
    pickle.dump(new_hash, open(pklpath, 'wb'))
