module.exports = function (grunt) {
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    settings: grunt.file.readJSON('settings.json'),
    sass: {
      dist: {
        files: {
          '../build/out/mobile/css/main.css': '../build/tmp/mobile/sass/main.scss'
        }
      }
    },
    concat: {
      options: {
        separator: ';',
      },
      mergeJs: {
        src: ['bower_components/milagro-crypto/js/DBIG.js', 'bower_components/milagro-crypto/js/BIG.js', 'bower_components/milagro-crypto/js/FP.js', 'bower_components/milagro-crypto/js/ROM.js', 'bower_components/milagro-crypto/js/HASH.js', 'bower_components/milagro-crypto/js/RAND.js', 'bower_components/milagro-crypto/js/AES.js', 'bower_components/milagro-crypto/js/GPM.js', 'bower_components/milagro-crypto/js/ECP.js', 'bower_components/milagro-crypto/js/FP2.js', 'bower_components/milagro-crypto/js/ECP2.js', 'bower_components/milagro-crypto/js/FP4.js', 'bower_components/milagro-crypto/js/FP12.js', 'bower_components/milagro-crypto/js/PAIR.js', 'bower_components/milagro-crypto/js/MPIN.js', 'bower_components/milagro-crypto/js/MPINAuth.js', '../libs/topLevelCode.js', 'bower_components/handlebars/handlebars.runtime.min.js', 'bower_components/qrcode/lib/qrcode.min.js'],
        dest: '../build/out/tmp/mobile/mpin-all.min.js'
      }
    },
    bgShell: {
      createDir: {
        cmd: 'mkdir -p ../build/out/mobile/js/ && mkdir -p ../build/out/',
        options: {
          stdout: true,
        }
      },
      createTemplate: {
        cmd: 'handlebars -n "mpin.templates" ./themes/<%= settings.templateName %>/views/*.handlebars -f ../build/out/mobile/js/templates.js',
        options: {
          stdout: true,
        }
      },
      bowerInstall: {
        cmd: 'bower install --allow-root',
        options: {
          stdout: true
        }
      },
      copyResources: {
        cmd: 'cp -Rv ./themes/<%= settings.templateName %>/images ../build/out/mobile/',
        options: {
          stdout: true,
        }
      },
      copyHandlebarsRuntime: {
        cmd: 'cp -Rv bower_components/handlebars/handlebars.runtime.min.js ../build/out/mobile/js/',
        options: {
          stdout: true,
        }
      },
      copySASS: {
        cmd: 'mkdir -p ../build/tmp/mobile/sass/templates/ && cp -Rv sass/* ../build/tmp/mobile/sass/ && cp themes/<%= settings.templateName %>/sass/_template.scss ../build/tmp/mobile/sass/templates/',
        options: {
          stdout: true,
        },
        done: function () {
          grunt.task.run('replace');
        }
      },
      copyJs: {
        cmd: 'cp -r ../build/out/tmp/mobile/mpin-all.min.js ../build/out/mobile/js/mpin-all.min.js',
        options: {
          stdout: true,
        }
      }
    },
    watch: {
      resourceFiles: {
        files: ['src/sass/*.scss', 'src/views/**/*.handlebars', 'js/*.js', 'settings.json', 'index.html'],
        tasks: ['bgShell', 'sass']
      }
    },
    replace: {
      dist: {
        options: {
          patterns: [
            {
              match: 'clientsetting',
              replacement: '<%= settings.clientSettingsURL %>'
            },
            {
              match: 'templatename',
              replacement: '<%= settings.templateName %>'
            },
            {
              match: 'emailregex',
              replacement: '<%= settings.emailRegex %>'
            },
            {
              match: 'timestamp',
              replacement: '<%= grunt.template.today() %>'
            }
          ]
        },
        files: [
          {expand: true, flatten: true, src: ['index.html'], dest: '../build/out/mobile/'},
          {expand: true, flatten: true, src: ['mpin.appcache'], dest: '../build/out/mobile/'},
          {expand: true, flatten: true, src: ['sass/main.scss'], dest: '../build/tmp/mobile/sass/'},
          {expand: true, flatten: true, src: ['themes/<%= settings.templateName %>/sass/_template.scss'], dest: '../build/tmp/mobile/sass/templates/'},
          {expand: true, flatten: true, src: ['js/main.js'], dest: '../build/out/mobile/js/'},
        ]
      }
    },
    uglify: {
      my_target: {
        files: {
          '../build/out/mobile/js/main.js': ['../build/out/mobile/js/main.js'],
          '../build/out/mobile/js/mpin-all.min.js': ['../build/out/tmp/mobile/mpin-all.min.js'],
          '../build/out/mobile/js/templates.js': ['../build/out/mobile/js/templates.js']
        }
      }
    },
    cssmin: {
      target: {
        files: {
          '../build/out/mobile/css/main.css': ['../build/out/mobile/css/main.css']
        }
      }
    },
  });
  grunt.loadNpmTasks('grunt-contrib-sass');
  grunt.loadNpmTasks('grunt-contrib-watch');
  grunt.loadNpmTasks('grunt-bg-shell');
  grunt.loadNpmTasks('grunt-contrib-uglify');
  grunt.loadNpmTasks('grunt-contrib-concat');
  grunt.loadNpmTasks('grunt-replace');
  grunt.loadNpmTasks('grunt-contrib-cssmin');
  grunt.loadNpmTasks('grunt-sass');


  grunt.registerTask('default', ['watch']);
  grunt.registerTask('build', ['bgShell', 'sass', 'concat', 'bgShell:copyJs']);
  grunt.registerTask('build-prod', ['bgShell', 'sass', 'concat', 'uglify', 'cssmin']);
}
