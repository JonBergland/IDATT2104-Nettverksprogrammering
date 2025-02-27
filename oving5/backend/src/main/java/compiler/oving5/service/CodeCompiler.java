package compiler.oving5.service;
import java.io.*;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


public class CodeCompiler {
    private static final Logger logger = LoggerFactory.getLogger(CodeCompiler.class);

    public String compileCode(String code) {
        try {
            return code;
            
        } catch (Exception e) {
            logger.error(code, e);
            throw e;
        }
    }

    public String runPythonCode(String code) {
        try {
            logger.info("Code received: {}",code);
            // String escapedCode = code.replace("\n", "\\n");

            File tempFile = File.createTempFile("code", ".py");
            try (BufferedWriter writer = new BufferedWriter(new FileWriter(tempFile))) {
                writer.write(code);
            }

            ProcessBuilder pb = new ProcessBuilder(
                "docker", "run", "--rm", "-v",
                tempFile.getAbsolutePath() + ":/tmp/code.py",
                "python-image", "python3", "/tmp/code.py");

            Process process = pb.start();

            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            BufferedReader errorReader = new BufferedReader(new InputStreamReader(process.getErrorStream()));
            String line;
            String output = "";
            while ((line = reader.readLine()) != null) {
                logger.info(line);
                output += line + '\n';
            }

            String errorOutput = "";
            while ((line = errorReader.readLine()) != null) {
                logger.error(line);
                errorOutput += line + '\n';
            }

            int exitCode = process.waitFor();
            System.out.println("Exited with code: " + exitCode + ", Error output: " + errorOutput);
            
            tempFile.delete();

            if (exitCode != 0) {
                return "Error: " + errorOutput;
            }

            return output;

        } catch (Exception e) {
            logger.error("Error in CodeCompiler: {}", e.getMessage());
            return e.getMessage();
        }
    }
}
