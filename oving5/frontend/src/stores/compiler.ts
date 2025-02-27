


export function compileCode(code:string) {

  const code1 = "print(\"Hello World\")"

  const command = 'docker run --rm python-image python3 -c ' + code1

  const result = executeCommand(command, (output: string) => {
    console.log(output);
  }, (error: string) => {
    console.error(error);
  });

  return result;

};
