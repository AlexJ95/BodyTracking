var project = new Project('BodyTracking', __dirname);

project.addFile('Sources/**');
project.setDebugDir('Deployment');

project.addIncludeDir('../HMM_Trainer/Sources');
project.addFile('../HMM_Trainer/Sources/kMeans.cpp');
project.addFile('../HMM_Trainer/Sources/kMeans.h');
project.addFile('../HMM_Trainer/Sources/Markov.cpp');
project.addFile('../HMM_Trainer/Sources/Markov.h');

project.addIncludeDir('Zulu/include');
project.addIncludeDir('Zulu/include/win32');
project.addLib('Zulu/lib/jvm');

resolve(project);
