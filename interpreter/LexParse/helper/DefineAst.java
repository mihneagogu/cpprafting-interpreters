import java.io.IOException;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.List;

class Main {
  public static void main(String[] args) throws IOException {
    if (args.length != 1) {
      System.err.println("Usage: generate_ast <output directory>");
      System.exit(64);
    }
    String outputDir = args[0];
    defineAst(outputDir, Arrays.asList("Binary   : Expr* left, Token operator, Expr* right",
                                       "Grouping : Expr* expression",
                                       "Literal  : void* value",
                                       "Unary    : Token operator, Expr* right"));
  }

  private static void defineAst(String outputDir, List<String> types) throws IOException {
    String headerPath = outputDir + "/Definitions.hpp";
    PrintWriter headerWriter = new PrintWriter(headerPath, "UTF-8");
    String cppPath = outputDir + "/Definitions.cpp";
    PrintWriter cppWriter = new PrintWriter(cppPath, "UTF-8");

    for (String type : types) {
      String className = type.split(":")[0].trim();
      String fields = type.split(":")[1].trim();
      cppWriter.println();
      headerWriter.println();
      defineType(headerWriter, className, fields);
      defineConstructor(cppWriter, className, fields);
    }
    cppWriter.close();
    headerWriter.close();
  }

  private static void defineConstructor(PrintWriter writer, String className, String fieldList) {
    String[] fields = fieldList.split(", ");
    writer.println(className + "::" + className + "(" + fieldList + ") {");
    for (String field : fields) {
      String name = field.split(" ")[1];
      writer.println("  this->" + name + " = " + name + ";");
    }
    writer.println("}");

  }

  private static void defineType(PrintWriter writer, String className, String fieldList) {
    writer.println("class " + className + "{");
    writer.println("  public: ");

    String[] fields = fieldList.split(", ");
    for (String field : fields) {
      writer.println("    " + field + ";");
    }

    // Print constructor definition;
    writer.print("    " + className + "::" + className + "(");
    for (int i = 0; i < fields.length; i++) {
      var field = fields[i];
      writer.print(field);
      if (i != fields.length - 1) {
        writer.print(", ");
      }
    }
    writer.println(");");

    // Print move constructor definition
    writer.print("    " + className + "::" + className + "(");
    writer.println(className + " &&to_move);");



    writer.println();

    writer.println("};");
  }
}
