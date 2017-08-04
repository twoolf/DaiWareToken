/**
 * Copyright 2017 StreamSets Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.streamsets.pipeline.api.impl.annotationsprocessor;

import com.streamsets.pipeline.api.ElDef;
import com.streamsets.pipeline.api.GenerateResourceBundle;
import com.streamsets.pipeline.api.StageDef;
import com.streamsets.pipeline.api.credential.CredentialStoreDef;
import com.streamsets.pipeline.api.impl.Utils;
import com.streamsets.pipeline.api.lineage.LineagePublisher;
import com.streamsets.pipeline.api.lineage.LineagePublisherDef;

import javax.annotation.processing.AbstractProcessor;
import javax.annotation.processing.ProcessingEnvironment;
import javax.annotation.processing.RoundEnvironment;
import javax.annotation.processing.SupportedAnnotationTypes;
import javax.annotation.processing.SupportedOptions;
import javax.annotation.processing.SupportedSourceVersion;
import javax.lang.model.SourceVersion;
import javax.lang.model.element.Element;
import javax.lang.model.element.TypeElement;
import javax.tools.Diagnostic;
import javax.tools.FileObject;
import javax.tools.StandardLocation;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;


@SupportedAnnotationTypes(
    {
        "com.streamsets.pipeline.api.StageDef",
        "com.streamsets.pipeline.api.GenerateResourceBundle",
        "com.streamsets.pipeline.api.ElDef",
        "com.streamsets.pipeline.api.lineage.LineagePublisherDef",
        "com.streamsets.pipeline.api.credential.CredentialStoreDef",
    })
@SupportedSourceVersion(SourceVersion.RELEASE_8)
@SupportedOptions(PipelineAnnotationsProcessor.SKIP_PROCESSOR)
public class PipelineAnnotationsProcessor extends AbstractProcessor {
  public static final String SKIP_PROCESSOR = "streamsets.datacollector.annotationsprocessor.skip";

  public static final String STAGES_FILE = "PipelineStages.json";
  public static final String LINEAGE_PUBLISHERS_FILE = "LineagePublishers.json";
  public static final String ELDEFS_FILE = "ElDefinitions.json";
  public static final String BUNDLES_FILE = "datacollector-resource-bundles.json";
  public static final String CREDENTIAL_STORE_FILE = "CredentialStores.json";

  private boolean skipProcessor;
  private final List<String> stagesClasses;
  private final List<String> lineagePublishersClasses;
  private final List<String> elDefClasses;
  private final List<String> bundleClasses;
  private final List<String> credentialStoreClasses;
  private boolean error;

  public PipelineAnnotationsProcessor() {
    super();
    stagesClasses = new ArrayList<>();
    elDefClasses = new ArrayList<>();
    bundleClasses = new ArrayList<>();
    lineagePublishersClasses = new ArrayList<>();
    credentialStoreClasses = new ArrayList<>();
  }

  @Override
  public synchronized void init(ProcessingEnvironment processingEnv) {
    super.init(processingEnv);
    skipProcessor = processingEnv.getOptions().containsKey(SKIP_PROCESSOR);
  }

  @Override
  public boolean process(Set<? extends TypeElement> annotations, RoundEnvironment roundEnv) {

    if (skipProcessor) {
      return true;
    }
    // Collect @StageDef classes
    for(Element e : roundEnv.getElementsAnnotatedWith(StageDef.class)) {
      if(e.getKind().isClass()) {
        stagesClasses.add(((TypeElement)e).getQualifiedName().toString());
      } else {
        printError("'{}' is not a class, cannot be @StageDef annotated", e);
        error = true;
      }
    }

    // Collect @LineagePublisherDef classes
    for(Element e : roundEnv.getElementsAnnotatedWith(LineagePublisherDef.class)) {
      if(e.getKind().isClass()) {
        lineagePublishersClasses.add(((TypeElement)e).getQualifiedName().toString());
      } else {
        printError("'{}' is not a class, cannot be @LineagePublisherDef annotated", e);
        error = true;
      }
    }

    // Collect @CredentialStoreDef classes
    for(Element e : roundEnv.getElementsAnnotatedWith(CredentialStoreDef.class)) {
      if(e.getKind().isClass()) {
        credentialStoreClasses.add(((TypeElement)e).getQualifiedName().toString());
      } else {
        printError("'{}' is not a class, cannot be @CredentialStoreDef annotated", e);
        error = true;
      }
    }

    // Collect @ElDef classes
    for(Element e : roundEnv.getElementsAnnotatedWith(ElDef.class)) {
      if(e.getKind().isClass()) {
        elDefClasses.add(((TypeElement)e).getQualifiedName().toString());
      } else {
        printError("'{}' is not a class, cannot be @ELDef annotated", e);
        error = true;
      }
    }

    // Collect @GenerateResourceBundle classes
    for(Element e : roundEnv.getElementsAnnotatedWith(GenerateResourceBundle.class)) {
      if(e.getKind().isClass()) {
        bundleClasses.add(((TypeElement) e).getQualifiedName().toString());
      } else {
        printError("'{}' is not a class, cannot be @GenerateResourceBundle annotated", e);
        error = true;
      }
    }

    // Generate files only if this is the last round and there is no error
    if(roundEnv.processingOver() && !error) {
      generateFiles();
    }
    return true;
  }

  private void printError(String template, Object... args) {
    processingEnv.getMessager().printMessage(Diagnostic.Kind.ERROR, Utils.format(template, args));
  }

  private void generateFiles() {
    generateFile(STAGES_FILE, stagesClasses);
    generateFile(ELDEFS_FILE, elDefClasses);
    generateFile(BUNDLES_FILE, bundleClasses);
    generateFile(LINEAGE_PUBLISHERS_FILE, lineagePublishersClasses);
    generateFile(CREDENTIAL_STORE_FILE, credentialStoreClasses);
  }

  static String toJson(List<String> elements) {
    StringBuilder sb = new StringBuilder();
    sb.append("[");
    String separator = "\n";
    for (String e : elements) {
      sb.append(separator).append("  \"").append(e).append('"');
      separator = ",\n";
    }
    sb.append("\n]\n");
    return sb.toString();
  }

  private void generateFile(String fileName, List<String> elements) {
    try {
      FileObject resource = processingEnv.getFiler().createResource(StandardLocation.CLASS_OUTPUT, "", fileName);
      try (Writer writer = new OutputStreamWriter(resource.openOutputStream())) {
        writer.write(toJson(elements));
      }
    } catch (IOException e) {
      printError("Could not create/write '{}' file: {}", e.toString());
    }
  }

}
