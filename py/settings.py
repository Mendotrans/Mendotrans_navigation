import os


class Config:
    DEBUG = os.getenv("FLASK_DEBUG", "false").lower() == "true"

    # Mendotran upstream
    MT_BASE_URL = os.getenv(
        "MT_BASE_URL", "https://owa.visionblo.com/api/mendoza")
    MT_TIMEOUT = int(os.getenv("MT_TIMEOUT", "10"))

    # Redis
    REDIS_HOST = os.getenv("REDIS_HOST", "localhost")
    REDIS_PORT = int(os.getenv("REDIS_PORT", "6379"))
    REDIS_DB = int(os.getenv("REDIS_DB", "0"))
    REDIS_PASSWORD = os.getenv("REDIS_PASSWORD", None)

    # TTL values

    # Search returns stops and services only, which don't change frequently
    CACHE_TTL_SEARCH = int(os.getenv("CACHE_TTL_SEARCH", "600"))  # 10m

    # Arrivals need to update almost live
    CACHE_TTL_ARRIVALS = int(os.getenv("CACHE_TTL_ARRIVALS", "30"))

    # Service trajectories are also pretty stable
    CACHE_TTL_TRAJECTORY = int(os.getenv("CACHE_TTL_TRAJECTORY", "300"))
