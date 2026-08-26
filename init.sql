CREATE TABLE messages (
    sender TEXT NOT NULL,
    content TEXT NOT NULL,
    friend_name TEXT NOT NULL,
    timestamp TEXT NOT NULL
);

CREATE TABLE friends (
    owner TEXT NOT NULL,
    friend_name TEXT NOT NULL,
    UNIQUE(owner, friend_name)
);
