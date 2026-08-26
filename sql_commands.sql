-- 插入消息
INSERT INTO messages
(sender, content, friend_name, timestamp)
VALUES (?, ?, ?, ?);

-- 查询两个用户之间的聊天记录
SELECT sender, content, friend_name, timestamp
FROM messages
WHERE
    (sender = ? AND friend_name = ?)
    OR
    (sender = ? AND friend_name = ?)
ORDER BY timestamp ASC;

-- 删除两个用户之间的聊天记录
DELETE FROM messages
WHERE
    (sender = ? AND friend_name = ?)
    OR
    (sender = ? AND friend_name = ?);

-- 添加好友
INSERT INTO friends
(owner, friend_name)
VALUES (?, ?);

-- 查询好友
SELECT friend_name
FROM friends
WHERE owner = ?;

-- 删除好友
DELETE FROM friends
WHERE owner = ?
AND friend_name = ?;
