package compiler.oving5.service;
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
    
}
