{
  'targets': [
    {
      'target_name': 'jp2a',
      'sources': [ 'addon.cc', 'Image.cc' ],
      'include_dirs': [],
      'conditions': [
        [ 'OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',       # -fno-exceptions
            'GCC_ENABLE_CPP_RTTI': 'YES',             # -fno-rtti
            'MACOSX_DEPLOYMENT_TARGET' : '10.7',      # from MAC OS 10.7
            'OTHER_CFLAGS': [
              '-DHAVE_CONFIG_H',
              '-O3',
              '-Wno-unused-private-field',
            ],
          },
          'libraries': ['-L/usr/local/lib -ljpeg -lncurses'],
          'include_dirs': [ '/usr/local/include' ],
        }, { # OS!="mac"
          'cflags!':    ['-fno-exceptions'],
          'cflags':     ['-DHAVE_CONFIG_H', '-O3'],
          'cflags_cc':  ['-Wall', '-O3', '-std=c++11', '-fexceptions'],
          'cflags_cc!': ['-fno-exceptions'],
          'cflags_cc!': ['-fno-rtti'],
          'libraries': ['-ljpeg -lncurses'],
        }],
      ]
    }
  ]
}
