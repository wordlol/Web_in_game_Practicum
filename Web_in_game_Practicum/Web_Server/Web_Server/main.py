from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from typing import Dict

#python -m uvicorn main:app --reload --port 8000

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


leaderboard: Dict[int, int] = {}

class ScoreSubmit(BaseModel):
    player_id: int
    score: int

@app.post("/leaderboard/submit")
async def submit_score(score: ScoreSubmit):
    current = leaderboard.get(score.player_id, 0)
    if score.score > current:
        leaderboard[score.player_id] = score.score
    return {"ok": True}

@app.get("/leaderboard/top10")
async def get_top10():
    sorted_players = sorted(leaderboard.items(), key=lambda x: x[1], reverse=True)[:10]
    return [{"player_id": pid, "score": score} for pid, score in sorted_players]

@app.get("/")
async def root():
    return {"message": "Game Leaderboard API is running"}