from building import *

CWD = GetCurrentDir()
sys.path.append(CWD)
import Sg

objs = Glob('src/*.c')


@register_library
class LibrarySg(Library):
    def useSDL(self):
        self.LIBS = ['SDL2']
        self.CPPDEFINES = ['USE_SDL']

    def useGTK(self):
        self.CPPDEFINES = ['USE_GTK']
        pkgcfg = 'pkg-config'
        if IsPlatformWindows():
            pkgcfg = '%s/mingw64/bin/pkg-config' % (os.getenv('MSYS2'))
        self.ParseConfig('%s --cflags gtk+-3.0' % (pkgcfg))
        self.ParseConfig('%s --libs gtk+-3.0 glib-2.0 gthread-2.0' % (pkgcfg))

    def config(self):
        self.include = '%s/include' % (CWD)
        self.CPPPATH = ['$INFRAS']
        # self.useSDL()
        self.useGTK()
        self.source = objs


sml = '%s/examples/cluster/Sg.xml' % (CWD)
Sg.GenerateSg(sml)
objsIC = Glob('examples/cluster/src/*.c') + Glob('examples/cluster/SgRes/*.c')


@register_library
class LibraryCluster(Library):
    def config(self):
        self.LIBS = ['Sg', 'Plugin']
        self.CPPPATH = ['$INFRAS', '$Com_Cfg']
        self.Append(CPPPATH=['%s/examples/cluster/SgRes' % (CWD)])
        self.source = objsIC


ApplicationCanApp = query_application('CanApp')


@register_application
class ApplicationCluster(ApplicationCanApp):
    def config(self):
        super().config()
        self.LIBS += ['Cluster']
        self.Append(CPPDEFINES=['USE_SG'])


sml = '%s/examples/watch/Sg.xml' % (CWD)
Sg.GenerateSg(sml)
objsWatch = Glob('examples/watch/src/*.c') + Glob('examples/watch/SgRes/*.c')


@register_library
class LibraryWatch(Library):
    def config(self):
        self.LIBS = ['Sg', 'Plugin']
        self.CPPPATH = ['$INFRAS']
        self.Append(CPPPATH=['%s/examples/watch/SgRes' % (CWD)])
        self.source = objsWatch

@register_application
class ApplicationWatch(ApplicationCanApp):
    def config(self):
        super().config()
        self.LIBS += ['Watch']
        self.Append(CPPDEFINES=['USE_SG'])