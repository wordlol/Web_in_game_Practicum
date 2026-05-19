<template>
  <v-app>
    <v-app-bar app color="primary" dark>
      <v-app-bar-title>
        Game Designer Admin Panel
      </v-app-bar-title>
      <v-spacer></v-spacer>
      <v-btn icon @click="refreshLeaderboard">
        <v-icon>mdi-refresh</v-icon>
      </v-btn>
    </v-app-bar>
    <v-main>
      <v-container>
        <v-row>
          <v-col cols="12" md="6">
            <ConfigPanel />
          </v-col>
          <v-col cols="12" md="6">
            <v-card>
              <v-card-title>
                Leaderboard Top 10
                <v-progress-circular v-if="loading" indeterminate size="24" class="ml-2"></v-progress-circular>
              </v-card-title>
              <v-card-text>
                <v-table density="compact">
                  <thead>
                    <tr>
                      <th>#</th>
                      <th>Player ID</th>
                      <th align="right">Score</th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr v-for="(item, idx) in leaderboard" :key="item.player_id">
                      <td>{{ idx + 1 }}</td>
                      <td>{{ item.player_id }}</td>
                      <td align="right">{{ item.score }}</td>
                    </tr>
                    <tr v-if="leaderboard.length === 0 && !loading">
                      <td colspan="3" class="text-center">No scores yet</td>
                    </tr>
                  </tbody>
                </v-table>
              </v-card-text>
            </v-card>
          </v-col>
        </v-row>
      </v-container>
    </v-main>
  </v-app>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import ConfigPanel from './components/ConfigPanel.vue'
import { leaderboardAPI } from './api'

const leaderboard = ref([])
const loading = ref(false)

const refreshLeaderboard = async () => {
  loading.value = true
  try {
    const res = await leaderboardAPI.getTop10()
    leaderboard.value = res.data
  } catch (err) {
    console.error(err)
  } finally {
    loading.value = false
  }
}

onMounted(refreshLeaderboard)
</script>