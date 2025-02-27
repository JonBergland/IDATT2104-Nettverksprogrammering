package compiler.oving5.controller;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import compiler.oving5.service.CodeCompiler;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.http.ResponseEntity;
import org.springframework.http.HttpStatus;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

@RestController
@RequestMapping("/api")
public class RestApiController {

    private static final Logger logger = LoggerFactory.getLogger(RestApiController.class);
    
    @GetMapping("/user")
    public String listAllUsers() {
        return "This works";
    }

    @PostMapping("/compile")
    public ResponseEntity<String> processCode(@RequestBody String input) {
        try {
            CodeCompiler compiler = new CodeCompiler();
            String result = compiler.runPythonCode(input);
            logger.info("Code compiled: {}", result);

            return ResponseEntity.ok(result);
            
        } catch (Exception e) {
            logger.error("Error processing equation", e);
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body("Error processing equation: " + e.getMessage());
        }

    }


}