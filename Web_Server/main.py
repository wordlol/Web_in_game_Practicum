#docker run --name some-postgres -e POSTGRES_PASSWORD=postgres -e POSTGRES_DB=game_db -d -p 5432:5432 postgres:15
#python -m uvicorn main:app --reload --port 8000


from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import asyncpg
import os

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

db_pool = None

class ScoreSubmit(BaseModel):
    player_id: int
    score: int

class WeaponDamage(BaseModel):
    weapon_id: str
    damage: int

# Только ОДИН эндпоинт PATCH
@app.patch("/config/weapon/{weapon_id}")
async def update_weapon_damage(weapon_id: str, data: WeaponDamage):
    async with db_pool.acquire() as conn:
        await conn.execute("""
            INSERT INTO game_config (key, value)
            VALUES ($1, $2)
            ON CONFLICT (key) DO UPDATE SET value = EXCLUDED.value
        """, f"weapon_{weapon_id}_damage", str(data.damage))
    return {"ok": True, "weapon_id": weapon_id, "damage": data.damage}
    

@app.on_event("startup")
async def startup():
    global db_pool
    database_url = os.getenv("DATABASE_URL", "postgresql://postgres:postgres@localhost:5432/game_db")
    db_pool = await asyncpg.create_pool(database_url, min_size=1, max_size=10)
    async with db_pool.acquire() as conn:
        await conn.execute("""
            CREATE TABLE IF NOT EXISTS leaderboard (
                player_id INTEGER PRIMARY KEY,
                score INTEGER NOT NULL,
                updated_at TIMESTAMP DEFAULT NOW()
            )
        """)
        await conn.execute("""
            CREATE TABLE IF NOT EXISTS game_config (
                key TEXT PRIMARY KEY,
                value TEXT NOT NULL,
                updated_at TIMESTAMP DEFAULT NOW()
            )
        """)
        # Добавляем значение по умолчанию для меча
        await conn.execute("""
            INSERT INTO game_config (key, value)
            VALUES ('weapon_sword_damage', '10')
            ON CONFLICT (key) DO NOTHING
        """)

@app.on_event("shutdown")
async def shutdown():
    if db_pool:
        await db_pool.close()

@app.post("/leaderboard/submit")
async def submit_score(score: ScoreSubmit):
    async with db_pool.acquire() as conn:
        await conn.execute("""
            INSERT INTO leaderboard (player_id, score)
            VALUES ($1, $2)
            ON CONFLICT (player_id) DO UPDATE
            SET score = EXCLUDED.score, updated_at = NOW()
            WHERE EXCLUDED.score > leaderboard.score
        """, score.player_id, score.score)
    return {"ok": True}

@app.get("/leaderboard/top10")
async def get_top10():
    async with db_pool.acquire() as conn:
        rows = await conn.fetch("""
            SELECT player_id, score
            FROM leaderboard
            ORDER BY score DESC
            LIMIT 10
        """)
    return [{"player_id": r["player_id"], "score": r["score"]} for r in rows]

@app.get("/config/weapon/{weapon_id}")
async def get_weapon_damage(weapon_id: str):
    async with db_pool.acquire() as conn:
        row = await conn.fetchrow(
            "SELECT value FROM game_config WHERE key = $1",
            f"weapon_{weapon_id}_damage"
        )
    if not row:
        raise HTTPException(404, "Weapon not found")
    return {"weapon_id": weapon_id, "damage": int(row["value"])}

@app.patch("/config/weapon/{weapon_id}")
async def update_weapon_damage(weapon_id: str, data: WeaponDamage):
    async with db_pool.acquire() as conn:
        await conn.execute("""
            INSERT INTO game_config (key, value)
            VALUES ($1, $2)
            ON CONFLICT (key) DO UPDATE SET value = EXCLUDED.value
        """, f"weapon_{weapon_id}_damage", str(data.damage))
    return {"ok": True, "weapon_id": weapon_id, "damage": data.damage}

@app.get("/")
async def root():
    return {"message": "Game API with config"}