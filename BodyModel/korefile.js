var project = new Project('BodyTracking', __dirname);

project.addFile('Sources/**');
project.setDebugDir('Deployment');

project.addIncludeDir('Zulu/include');
project.addIncludeDir('Zulu/include/win32');
project.addLib('Zulu/lib/jvm');

resolve(project);
