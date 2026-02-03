---
disableNunjucks: true
mathjax: false
title: MongoDB语法速查
tags:
  - JavaScript
  - MongoDB
  - 数据库
categories:
  - 瞎折腾
description: >-
  这篇文章介绍了作者在维护采用MongoDB作为数据库的项目过程中，对数据库、集合及增删改查文档等基础操作的学习和记录，包括如何创建、删除和清空数据库及集合，以及如何查询、插入、更新和删除文档的具体实践。
date: 2022-11-01 10:28:30
---

> 近几天接手维护的项目采用MongDB作为数据库。
速度学习了一下用法，记录一下。

<!-- more -->

### 数据库操作

#### 进入/创建数据库

```sql
use myDatabase;
```


#### 删除数据库

```javascript
db.dropDatabase('myDatabase');
```

### 集合操作

#### 创建集合

```javascript
db.createCollection('myCollection');
```

#### 删除集合

```javascript
db.myCollection.drop();
```

#### 清空集合

```javascript
db.myCollection.remove({}); // outdated
db.myCollection.deleteMany({});
```

### 增删改查文档

#### 查询文档

```javascript
// 查找 id = 1 的一个数据
db.myCollection.findOne({id: 1});

// 查找 id = 1 的一个数据的name字段
db.myCollection.findOne({id: 1}).name;

// 查找 id > 3 的所有数据
db.myCollection.find({id: {$gt: 3}});

// 查找 id > 10 并且 name == 'John'的所有数据
db.myCollection.find({id: {$gt: 10}, name: 'John'});
```

#### 插入文档

```javascript
// 插入一个数据
db.myCollection.insertOne({id: 1, name: 'Julia'});

// 插入多个数据
// 注意，id 可以重复，但是后端程序不一定这样认为，就会报错
db.myCollection.insertOne({id: 1, name: 'Ruby'}, {id: 1, name: 'Sapphire'});
```

#### 更新文档

```javascript
// 将 id > 5 的文档的 name 字段都设为 'Julia'
db.myCollection.update(
  {
    id: { $lt: 5 }
  },
  {
    $set: { name: 'Julia' }
  }
);
```

#### 删除文档


```javascript
// 删除一个 id = 1 的文档
db.myCollection.deleteOne({id: 1});

// 删除所有 id > 10 的文档
db.myCollection.deleteMany({id: {$gt: 10}});
```

<!-- Finish -->
[没了]{.display-block .text-indent-none .mt-4 .mb-2 .fw-600}
