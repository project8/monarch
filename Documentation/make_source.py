# Based on: https://github.com/seqan/seqan3-doc/blob/user/source/make_source.py
# Copied Jan 3, 2018

#  written by Jongkyu Kim (j.kim@fu-berlin.de)

import os
import sys

CAT_NAME = "API Reference"
INDEX_TEMP = "./_index.rst"
API_DIR = "API_Ref"
TOC_END = ".. end of toc\n"

def generateIndex(outDir):
    print('[make_source] Generating index')
    #listModules = []
    #for fileName in os.listdir(inDir) :
    #    if os.path.isdir(inDir + fileName) == True and fileName.startswith('Monarch'):
    #        print '[make_source] Found module', fileName
    #        listModules.append(fileName)  
    #listModules = sorted(listModules)
    #print("[make_source] list of modules is: {}".format(listModules))

    # generate index.rst
    inFile = open(INDEX_TEMP, "r")
    outFile = open(outDir + "/index.rst", "w")
    for line in inFile :
        outFile.write(line)
    inFile.close()
    outFile.close()

    # generate API_Ref/index.rst
    api_path = os.path.join(outDir, API_DIR)
    mkAPISubdir(api_path, API_DIR)

def addTOCEntry(toc_path, new_item, caption=None):
    the_lines = open(toc_path).readlines()
    end_of_tree = len(the_lines)
    try:
        end_of_tree = the_lines.index(TOC_END)
    except ValueError:
        print("[make_source] WARNING!! someone removed the end of ToC comment marker!\n     the jerk ... assuming EOF")
    the_lines.insert(end_of_tree, "   {}\n".format(new_item))
    if caption is not None:
        the_lines.insert(end_of_tree, "   :caption: {}\n".format(caption))
    f= open(toc_path, "w")
    f.writelines(the_lines)
    f.close()

def mkAPISubdir(dirname, title, caption=True):
    print("Making subdir: {}".format(dirname))
    if caption == True:
        caption = title
        print('>> caption will be: {}'.format(caption))
    # make the directory
    if not os.path.isdir(dirname):
        os.mkdir(dirname)
    # make the index file in the directory
    api_toc_file = open(os.path.join(dirname, "index.rst"), 'w')
    api_toc_file.write("{}\n{}\n\n".format(title, '='*len(title)))
    api_toc_file.write(".. toctree::\n   :maxdepth: 2\n\n")
    api_toc_file.write(TOC_END)
    api_toc_file.close()
    # add the subdir's index to the parent's ToC
    print("adding {} as {}".format(dirname, os.path.relpath(dirname)))
    addTOCEntry(os.path.join(dirname, '../index.rst'), '{}/index'.format(os.path.relpath(dirname).split('/')[-1]), caption=caption)

def generateFileRST(outDir, moduleName, fileName) :
    print('[make_source] Generating File RST:', outDir, moduleName, fileName)

    # title
    out_file_path = os.path.join(outDir,fileName.split('../',1)[-1]) + ".rst"
    print('writing to: {}'.format(out_file_path))
    outFile = open(out_file_path, "w")
    title = '/'.join([moduleName, fileName])
    outFile.write(title + "\n")
    outFile.write("=" * len(title) + "\n\n")

    # doxgenfile
    outFile.write(".. doxygenfile:: %s/%s\n" % (outDir[2:], fileName))
    outFile.write("   :project: myproject\n\n")

    outFile.close()

def generateRST(outDir, moduleName, listModules, listFiles) :
    return
    #print('WARNING [make_source] Generating RST: {} {} {} {}'.format(outDir, moduleName, listModules, listFiles))
    #if len(listModules) > 0 and os.path.isdir(outDir) == False:
    #    os.mkdir(outDir)

    ## title
    #outFile = open(outDir + ".rst","w")
    #outFile.write(moduleName[0].upper() + moduleName[1:] + "\n")
    #outFile.write("=" * len(moduleName) + "\n\n")

    ## doxygenfile
    ##for fileName in listFiles :
    ##    outFile.write(".. doxygenfile:: %s/%s\n" % (outDir[3:], fileName))
    ##    outFile.write("   :project: myproject\n\n")

    ## toctree
    #outFile.write(".. toctree::\n")
    #outFile.write("   :caption: %s:\n" % CAT_NAME)
    #outFile.write("   :titlesonly:\n")
    #outFile.write("   :maxdepth: 1\n")
    ##outFile.write("   :hidden:\n\n")
    #for childModuleName in listModules :
    #   outFile.write("   %s/%s\n" % (moduleName, childModuleName) )
    #outFile.close()

def generateRSTs(in_names, outDir):
    print('[make_source] Generating RSTs: {} {}'.format(in_names, outDir))
    listModules = []
    listFiles = []
    #print("[make_source] generate for:\n{}".format(os.listdir(inDir)))
    for fileName in in_names:
        if os.path.isdir(fileName):
            listModules.append(fileName)
        elif fileName.endswith(('.hh', '.cc')):
            listFiles.append(fileName)

    listModules = sorted(listModules)
    listFiles = sorted(listFiles)
    print("[make_source] modules: {}".format(listModules))
    print("[make_source] files: {}".format(listFiles))

    api_dir = os.path.join(outDir, API_DIR)
    moduleName = ''
    for filename in listFiles :
        print('[make_source] (api_dir, module, file) -> ({}, {}, {})'.format(api_dir, moduleName, fileName))
        generateFileRST(api_dir, moduleName, fileName)

    for a_module in listModules:
        print('submodule: {}'.format(a_module))
        #print('submodule split: {}'.format(a_module.split('../',1)[-1]))
        a_name = a_module.split('../', 1)[-1]
        mkAPISubdir(os.path.join(api_dir, a_name), a_name)
        generateRSTs([os.path.join(a_module, f) for f in os.listdir(a_module)], outDir)

    #for moduleName in listModules :
    #    curInDir = inDir + "/" + moduleName
    #    curOutDir = outDir + "/" + moduleName
    #    generateRSTs(curInDir, curOutDir, False)


###################

# usage: python make_source.py <path_to_where_doc_rst_files_live> <paths_to_each_directory_containing_source_with_doxygen>

# you should run this file early from within your conf.py

outDir = sys.argv[1]
inDirs = sys.argv[2:]

generateIndex(outDir)
generateRSTs(inDirs, outDir)
