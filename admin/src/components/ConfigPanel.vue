<template>
  <v-card variant="outlined" class="pa-4">
    <v-card-title class="text-h6">
      <v-icon icon="mdi-sword" class="mr-2"></v-icon>
      Game Balance Config
    </v-card-title>
    <v-card-text>
      <v-slider
        v-model="damage"
        :min="1"
        :max="100"
        label="Sword Damage"
        thumb-label
        color="primary"
        @update:model-value="debouncedUpdate"
      ></v-slider>
      <v-text-field
        v-model="damage"
        type="number"
        label="Exact value"
        variant="outlined"
        density="compact"
        @input="debouncedUpdate"
      ></v-text-field>
      <v-btn
        color="primary"
        @click="save"
        :loading="saving"
        block
        class="mt-2"
      >
        Save Changes
      </v-btn>
      <v-alert
        v-if="saved"
        type="success"
        variant="tonal"
        class="mt-4"
      >
        Settings saved successfully!
      </v-alert>
    </v-card-text>
  </v-card>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { configAPI } from '../api'

const damage = ref(10)
const saving = ref(false)
const saved = ref(false)

const save = async () => {
    saving.value = true
    try {
        const roundedDamage = Math.round(damage.value)   // округляем до целого
        await configAPI.updateWeaponDamage('sword', roundedDamage)
        damage.value = roundedDamage                     // обновляем локальное значение
        saved.value = true
        setTimeout(() => saved.value = false, 2000)
    } catch (err) {
        console.error(err)
    } finally {
        saving.value = false
    }
}

let timeoutId
const debouncedUpdate = () => {
  clearTimeout(timeoutId)
  timeoutId = setTimeout(() => save(), 1000)
}

onMounted(async () => {
  try {
    const res = await configAPI.getWeaponDamage('sword')
    damage.value = res.data.damage
  } catch (err) {
    console.error('Failed to load config', err)
  }
})
</script>