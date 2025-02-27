import axiosInstance from '../plugins/axios'


export async function compileCode(code:string): Promise<String> {
  console.log("Compiling code");


  // const code1 = "print('Hello World')"

  // const command = 'docker run --rm python-image python3 -c ' + code1

  try {
    const response = await axiosInstance.post("/compile", code)
    console.log(response.data)

    return response.data

  } catch (error) {
    console.log("Error compiling the code ", error)
    return JSON.stringify(error)
  }
};
