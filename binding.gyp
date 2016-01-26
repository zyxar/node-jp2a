{
  'targets': [
    {
      'target_name': 'jp2a',
      'sources': [ 'addon.cc', 'jp2a-1.0.6/src/html.c', 'jp2a-1.0.6/src/image.c', 'jp2a-1.0.6/src/options.c', 'jp2a-1.0.6/src/term.c', 'jp2a-1.0.6/src/curl.c'],
      'include_dirs': [ '/opt/include', 'jp2a-1.0.6/include', 'jp2a-1.0.6' ],
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
          'libraries': ['-ljpeg -lcurl -lncurses'],
        }, { # OS!="mac"
          'cflags!':    ['-fno-exceptions'],
          'cflags':     ['-DHAVE_CONFIG_H', '-O3'],
          'cflags_cc':  ['-Wall', '-O3', '-std=c++11', '-fexceptions'],
          'cflags_cc!': ['-fno-exceptions'],
          'cflags_cc!': ['-fno-rtti'],
          'libraries': ['-ljpeg -lcurl -lncurses'],
        }],
      ]
    }
  ]
}
