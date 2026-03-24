# **Distributed Key-Value Log System**

## 📌 Overview

I am building from scratch a distributed system using the leader-follower pattern. The main goal is to replicate
a key-value log over all network nodes ensuring data replication, data consistency, data availability and fault tolerance. All system code is made from scratch, trying to approach real word solutions but on a small scale.

At the moment, I am working on the project, so it is not finished.

## 🎯 Current Objectives

- Apply replication.
- Design consensus algorithm.
- Consistency.
- System network foundations.
- Fault tolerance -> Node faults, node recovery, leader faults, network partitions...
- CAP theorem.

## ✅ Features That Work

- Data writes by leader (Data state and key-value log).
- Data replication to followers.
- Message communication protocol.
- System network.
- Synchronous replication.