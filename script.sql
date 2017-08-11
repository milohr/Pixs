
CREATE TABLE IF NOT EXISTS sources (
  url VARCHAR(150) PRIMARY KEY
);


CREATE TABLE IF NOT EXISTS albums (
    title VARCHAR(30) PRIMARY KEY

);

CREATE TABLE IF NOT EXISTS images (
    url VARCHAR(150) PRIMARY KEY,
    title VARCHAR(30) NOT NULL,
    rate INTEGER NOT NULL,
    fav INTEGER NOT NULL,
    color VARCHAR(9),
    note TEXT,
    addDate DATE,
    source VARCHAR(150) NOT NULL,
    FOREIGN KEY(source) REFERENCES sources(url)

);

CREATE TABLE IF NOT EXISTS images_albums (
    albumTitle VARCHAR(30) NOT NULL,
    imageUrl VARCHAR(150) NOT NULL,
    FOREIGN KEY(albumTitle) REFERENCES albums(title),
    FOREIGN KEY(imageUrl) REFERENCES images(url)
);
