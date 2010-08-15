#define OTHER_LIBS interrogatedb:c dconfig:c dtoolconfig:m \
                   dtoolutil:c dtoolbase:c dtool:m prc:c

#begin lib_target
  #define TARGET p3collada
  #define LOCAL_LIBS display express putil pandabase dxml char

  #define COMBINED_SOURCES collada_composite1.cxx collada_composite2.cxx

  #define SOURCES \
    colladaAccessor.h colladaAccessor.I \
    colladaArray.h colladaArray.I colladaArray.N \
    colladaAsset.h colladaAsset.I \
    colladaAssetElement.h colladaAssetElement.I \
    colladaCamera.h colladaCamera.I \
    colladaContributor.h colladaContributor.I \
    colladaController.h \
    colladaDocument.h colladaDocument.I \
    colladaEffect.h colladaEffect.I \
    colladaElement.h colladaElement.I \
    colladaGeometry.h \
    colladaInput.h colladaInput.I \
    colladaInstance.h colladaInstance.I colladaInstance.N \
    colladaLibrary.h colladaLibrary.I colladaLibrary.N \
    colladaLight.h colladaLight.I \
    colladaMaterial.h colladaMaterial.I \
    colladaMesh.h colladaMesh.I \
    colladaNode.h colladaNode.I \
    colladaOptics.h colladaOptics.I \
    colladaPrimitive.h colladaPrimitive.I \
    colladaProfile.h colladaProfile.I \
    colladaRoot.h colladaRoot.I \
    colladaSource.h colladaSource.I \
    colladaVertices.h \
    colladaVisualScene.h colladaVisualScene.I \
    config_collada.h \
    load_collada_file.h \
    loaderFileTypeDae.h

  #define INCLUDED_SOURCES \
    colladaAccessor.cxx \
    colladaArray.cxx \
    colladaAsset.cxx \
    colladaAssetElement.cxx \
    colladaCamera.cxx \
    colladaContributor.cxx \
    colladaController.cxx \
    colladaDocument.cxx \
    colladaEffect.cxx \
    colladaGeometry.cxx \
    colladaInput.cxx \
    colladaInstance.cxx \
    colladaLibrary.cxx \
    colladaLight.cxx \
    colladaMaterial.cxx \
    colladaMesh.cxx \
    colladaNode.cxx \
    colladaOptics.cxx \
    colladaPrimitive.cxx \
    colladaProfile.cxx \
    colladaRoot.cxx \
    colladaSource.cxx \
    colladaVertices.cxx \
    colladaVisualScene.cxx \
    config_collada.cxx \
    load_collada_file.cxx \
    loaderFileTypeDae.cxx

  #define INSTALL_HEADERS \
    colladaAccessor.h colladaAccessor.I \
    colladaArray.h colladaArray.I \
    colladaAsset.h colladaAsset.I \
    colladaAssetElement.h colladaAssetElement.I \
    colladaCamera.h colladaCamera.I \
    colladaContributor.h colladaContributor.I \
    colladaController.h \
    colladaDocument.h colladaDocument.I \
    colladaEffect.h colladaEffect.I \
    colladaElement.h colladaElement.I \
    colladaGeometry.h \
    colladaInput.h colladaInput.I \
    colladaInstance.h colladaInstance.I \
    colladaLibrary.h colladaLibrary.I \
    colladaLight.h colladaLight.I \
    colladaMaterial.h colladaMaterial.I \
    colladaMesh.h colladaMesh.I \
    colladaNode.h colladaNode.I \
    colladaOptics.h colladaNode.I \
    colladaPrimitive.h colladaPrimitive.I \
    colladaProfile.h colladaProfile.I \
    colladaRoot.h colladaRoot.I \
    colladaSource.h colladaSource.I \
    colladaVertices.h \
    colladaVisualScene.h colladaVisualScene.I \
    config_collada.h \
    load_collada_file.h \
    loaderFileTypeDae.h

  #define IGATESCAN all

#end lib_target
