import axios from 'axios'

const api = axios.create({
    baseURL: '/api',
    headers: { 'Content-Type': 'application/json' }
})

export const leaderboardAPI = {
    getTop10: () => api.get('/leaderboard/top10')
}

export const configAPI = {
    getWeaponDamage: (weaponId) => api.get(`/config/weapon/${weaponId}`),
    updateWeaponDamage: (weaponId, damage) =>
        api.patch(`/config/weapon/${weaponId}`, { weapon_id: weaponId, damage: damage })
}

//npm install --legacy-peer-deps
