<script setup lang="ts">
import IdeItem from '@/components/IdeItem.vue'
import { compileCode } from '@/stores/compiler';

async function sendCode() {
  console.log("Button clicked");

  const code = document.querySelector('#ide-area') as HTMLTextAreaElement

  const output = document.querySelector('#output') as HTMLParagraphElement
  output.innerHTML = "Output:<br>"

  const codeText = code.value;
  console.log(codeText);

  const solvedCode = await compileCode(codeText)

  output.innerHTML += solvedCode + '<br>'

  return code
}
</script>

<template>
  <div>
    <h3>IDE for Python</h3>
    <textarea
      name="ide"
      id="ide-area"
      cols="40"
      rows="7"
      ></textarea>
  </div>
  <div>
    <button class="compile-run" @click="sendCode">Compile and Run</button>
    <p id="output">Output:<br></p>
  </div>
</template>
