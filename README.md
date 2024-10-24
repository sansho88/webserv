<p align="center">
    <h1 align="center">WEBSERV</h1>
</p>
<p align="center">
    <em><code>❯ Serveur Web entierement fait en C++ 98</code></em>
</p>
<p align="center">
	<img src="https://img.shields.io/github/license/sansho88/webserv?style=for-the-badge&logo=opensourceinitiative&logoColor=white&color=0080ff" alt="license">
	<img src="https://img.shields.io/github/last-commit/sansho88/webserv?style=for-the-badge&logo=git&logoColor=white&color=0080ff" alt="last-commit">
	<img src="https://img.shields.io/github/languages/top/sansho88/webserv?style=for-the-badge&color=0080ff" alt="repo-top-language">
	<img src="https://img.shields.io/github/languages/count/sansho88/webserv?style=for-the-badge&color=0080ff" alt="repo-language-count">
</p>
<p align="center">
		<em>Built with the tools and technologies:</em>
</p>
<p align="center">
	<img src="https://img.shields.io/badge/HTML5-E34F26.svg?style=for-the-badge&logo=HTML5&logoColor=white" alt="HTML5">
	<img src="https://img.shields.io/badge/C-A8B9CC.svg?style=for-the-badge&logo=C&logoColor=black" alt="C">
	<img src="https://img.shields.io/badge/PHP-777BB4.svg?style=for-the-badge&logo=PHP&logoColor=white" alt="PHP">
	<img src="https://img.shields.io/badge/GitHub%20Actions-2088FF.svg?style=for-the-badge&logo=GitHub-Actions&logoColor=white" alt="GitHub%20Actions">
</p>

<br>

##### 🔗 Table of Contents

- [📍 Overview](#-overview)
- [👾 Features](#-features)
- [📂 Repository Structure](#-repository-structure)
- [🚀 Getting Started](#-getting-started)
    - [🔖 Prerequisites](#-prerequisites)
    - [📦 Installation](#-installation)
    - [🤖 Usage](#-usage)
---

## 📍 Overview

<code>
Webserv est un projet visant à créer un serveur HTTP en C++.
Ce serveur permettra de comprendre le fonctionnement du protocole HTTP, qui est essentiel pour la communication sur le World Wide Web.
L'objectif principal est de développer un serveur capable de traiter des requêtes HTTP et de répondre correctement aux clients.
</code>

---

## 👾 Features

<code>
❯ Support des Méthodes HTTP : Implémentation des méthodes GET, POST et DELETE.
❯ Gestion des Requêtes : Analyse et traitement des requêtes HTTP en fonction des spécifications du protocole.
❯ Pages d'Erreur : Fournir des pages d'erreur par défaut en cas de problème lors du traitement des requêtes.
❯ Fichier de Configuration : Prise en charge d'un fichier de configuration pour définir les paramètres du serveur (port, hôte, routes, etc.).
❯ Serveur Non-bloquant : Utilisation de poll() ou d'un équivalent pour gérer plusieurs connexions de manière non-bloquante.
❯ Upload de Fichiers : Permettre aux clients de téléverser des fichiers.</code>

---

## 📂 Repository Structure

```sh
└── webserv/
    ├── .github
    ├── Cgi
    ├── Config
    ├── DirectoryListing.cpp
    ├── DirectoryListing.h
    ├── Http
    ├── IO
    ├── Makefile
    ├── MimeDatabase.cpp
    ├── MimeDatabase.h
    ├── MountPoint.cpp
    ├── MountPoint.h
    ├── Network
    ├── Regex
    ├── ServerHost.cpp
    ├── ServerHost.h
    ├── VirtualHost.cpp
    ├── VirtualHost.h
    ├── Webserv.cpp
    ├── Webserv.h
    ├── ccgi
    ├── ccgi.c
    ├── htdocs
    ├── main.cpp
    ├── string_utils.hpp
    └── webserv.conf
```

## 🚀 Getting Started

### 🔖 Prerequisites

**C++**: `version 98`
Be sure to have "make" installed on your computer

```sh
sudo apt install make
```

### 📦 Installation

Build the project from source:

1. Clone the webserv repository:
```sh
git clone https://github.com/sansho88/webserv
cd webserv
```

2. Compile the project:
```sh
make
```

3. Server launch:
```sh
./webserv <fichier_de_configuration>
```

### 🤖 Usage

To run the project, execute the following command:

```sh
❯ ./webserv
```
