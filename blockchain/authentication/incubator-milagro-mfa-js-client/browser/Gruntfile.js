module.exports = function (grunt) {
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    settings: grunt.file.readJSON('settings.json'),
    sass: {
      dist: {
        files: {
          '../build/out/browser/css/main.css': '../build/out/tmp/browser/sass/main.scss'
        }
      }
    },
    concat: {
      options: {
        separator: ';'
      },
      mergeJs: {
        src: ['bower_components/milagro-crypto/js/DBIG.js', 'bower_components/milagro-crypto/js/BIG.js', 'bower_components/milagro-crypto/js/FP.js', 'bower_components/milagro-crypto/js/ROM.js', 'bower_components/milagro-crypto/js/HASH.js', 'bower_components/milagro-crypto/js/RAND.js', 'bower_components/milagro-crypto/js/AES.js', 'bower_components/milagro-crypto/js/GPM.js', 'bower_components/milagro-crypto/js/ECP.js', 'bower_components/milagro-crypto/js/FP2.js', 'bower_components/milagro-crypto/js/ECP2.js', 'bower_components/milagro-crypto/js/FP4.js', 'bower_components/milagro-crypto/js/FP12.js', 'bower_components/milagro-crypto/js/PAIR.js', 'bower_components/milagro-crypto/js/MPIN.js', 'bower_components/milagro-crypto/js/MPINAuth.js', '../libs/topLevelCode.js', 'bower_components/handlebars/handlebars.runtime.min.js', 'bower_components/qrcode/lib/qrcode.min.js', 'bower_components/handlebars/handlebars.runtime.min.js', 'bower_components/qrcodejs/qrcode.min.js', '../build/out/tmp/browser/templates.js', 'bower_components/milagro-mfa-js-lib/lib/mpin.js', 'js/mpin.js'],
        dest: '../build/out/tmp/browser/mpin-all.js'
      }
    },
    clean: {
      build: {
        cwd: '../build/',
        files: 'out',
        options: {
          force: true
        }
      }
    },
    bgShell: {
      createDir: {
        cmd: "mkdir -p ../build/out/tmp/browser",
        options: {
          stdout: true
        }
      },
      createTemplate: {
        cmd: 'handlebars ./themes/<%= settings.templateName %>/views/*.handlebars -f ../build/out/tmp/browser/templates.js',
        options: {
          stdout: true
        }
      },
      bowerInstall: {
        cmd: 'bower install --allow-root',
        options: {
          stdout: true
        }
      },
      replaceURLBASE: {
        cmd: "sed 's#%URL_BASE%#<%= settings.URLBase %>#' ../build/out/tmp/browser/mpin-all.js > ../build/out/tmp/browser/mpin-url.js",
        options: {
          stdout: true
        }
      },
      replaceTEMPLATENAME: {
        cmd: "sed 's#%TEMPLATE_NAME%#<%= settings.templateName %>#' ../build/out/tmp/browser/mpin-url.js > ../build/out/tmp/browser/mpin.js",
        options: {
          stdout: true,
        }
      },
      mkdirTmpSass: {
        cmd: "mkdir -p ../build/out/tmp/browser/sass/templates",
        options: {
          stdout: true
        }
      },
      copyTmpSass: {
        cmd: "cp -r sass/* ../build/out/tmp/browser/sass/",
        options: {
          stdout: true
        }
      },
      copyTmpFile: {
        cmd: "cp -r themes/<%= settings.templateName %>/sass/_template.scss ../build/out/tmp/browser/sass/templates/",
        options: {
          stdout: true
        }
      },
      createFolders: {
        cmd: 'mkdir -p ../build/out/browser/images',
        options: {
          stdout: true
        }
      },
      copyResources: {
        cmd: 'cp -r themes/<%= settings.templateName %>/images/* ../build/out/browser/images/',
        options: {
          stdout: true
        }
      },
      copyJs: {
        cmd: 'cp -r ../build/out/tmp/browser/mpin.js ../build/out/browser/',
        options: {
          stdout: true
        }
      }
    },
    uglify: {
      my_target: {
        files: {
          '../build/out/browser/mpin.js': ['../build/out/browser/mpin.js']
        }
      }
    },
    hashres: {
      options: {
        fileNameFormat: '${hash}.${name}.${ext}'
      },
      changeCss: {
        expand: true,
        cwd: '../build/out/browser/css',
        src: 'main.css',
        dest: 'mpin.js',
        rename: function (d) {
          return '../build/out/browser/mpin.js'
        }
      }
    },
    cssmin: {
      target: {
        files: [{
            expand: true,
            cwd: '../build/out/browser',
            src: ['css/main.css'],
            dest: 'css/main.css'
          }]
      }
    },
    watch: {
      css: {
        files: ['src/sass/*.scss', 'src/sass/templates/*.scss'],
        tasks: ['sass']
      },
      views: {
        files: 'src/views/*.html',
        tasks: ['bgShell:makeViews']
      }
    }
  });


  grunt.loadNpmTasks('grunt-contrib-watch');
  grunt.loadNpmTasks('grunt-bg-shell');
  grunt.loadNpmTasks('grunt-contrib-uglify');
  grunt.loadNpmTasks('grunt-contrib-concat');
  grunt.loadNpmTasks('grunt-hashres');
  grunt.loadNpmTasks('grunt-replace');
  grunt.loadNpmTasks('grunt-sass');
  grunt.loadNpmTasks('grunt-contrib-cssmin');
  grunt.loadNpmTasks('grunt-contrib-clean');

  grunt.registerTask('default', ['watch']);
  grunt.registerTask('build', ['clean:build', 'bgShell:createDir', 'bgShell:createTemplate', 'bgShell:bowerInstall', 'concat', 'bgShell', 'sass', 'bgShell:copyJs', 'hashres']);
  grunt.registerTask('build-prod', ['clean:build', 'bgShell:createDir', 'bgShell:createTemplate', 'bgShell:bowerInstall', 'concat', 'bgShell', 'sass', 'cssmin', 'bgShell:copyJs', 'hashres', 'uglify']);
}
