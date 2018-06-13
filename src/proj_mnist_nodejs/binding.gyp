{
    'targets': [
    {
        'target_name': 'mlp',
        'sources': [
            '../ann/mlp.cpp',
            'main.cpp'
        ],
        'cflags': [
            '-std=c++14',
            '-fexceptions',
            '-DNODEJS'
        ],
        'include_dirs': [
            '../../include',
            '../ann'
        ],
        'libraries': [
            '-lboost_system'
        ],
    }
  ]
}
