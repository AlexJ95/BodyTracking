var project = new Project('BodyTracking', __dirname);

project.addFile('Sources/**');
project.setDebugDir('Deployment');

project.addIncludeDir('../HMM_Trainer/Sources');
project.addFile('../HMM_Trainer/Sources/kMeans.cpp');
project.addFile('../HMM_Trainer/Sources/kMeans.h');
project.addFile('../HMM_Trainer/Sources/Markov.cpp');
project.addFile('../HMM_Trainer/Sources/Markov.h');

project.addIncludeDir('C:/Program Files/Zulu/zulu-10/include');
project.addIncludeDir('C:/Program Files/Zulu/zulu-10/include/win32');
project.addLib('C:/Program Files/Zulu/zulu-10/lib/jvm');

resolve(project);
