-- MySQL dump 10.16  Distrib 10.1.33-MariaDB, for Linux (x86_64)
--
-- Host: localhost    Database: anon
-- ------------------------------------------------------
-- Server version	10.1.33-MariaDB

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `clients`
--

DROP TABLE IF EXISTS `clients`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `clients` (
  `pid` int(11) DEFAULT NULL,
  `id` int(11) DEFAULT NULL,
  KEY `pid` (`pid`),
  CONSTRAINT `clients_ibfk_1` FOREIGN KEY (`pid`) REFERENCES `peers` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `clients`
--

LOCK TABLES `clients` WRITE;
/*!40000 ALTER TABLE `clients` DISABLE KEYS */;
/*!40000 ALTER TABLE `clients` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `peers`
--

DROP TABLE IF EXISTS `peers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `peers` (
  `sno` int(11) DEFAULT NULL,
  `id` int(11) NOT NULL,
  `ip` varchar(20) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ku` varchar(4069) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `peers`
--

LOCK TABLES `peers` WRITE;
/*!40000 ALTER TABLE `peers` DISABLE KEYS */;
INSERT INTO `peers` VALUES (0,4107,'192.168.225.53','-----BEGIN PUBLIC KEY-----\nMIIEIjANBgkqhkiG9w0BAQEFAAOCBA8AMIIECgKCBAEA0BHrdK2NHm6dHhaUQnez\nqBjuZvJvKVQRPajcsSiAG7vRG8nYI60QehuUx4wWuPnRHlGgq823GY9gwYmm7FJL\noj8iYpuvvvbRTGPy2gz8vl7LyEn6xUCZeO4oOtm2xXgebN21KhJEgfkI7l3ZItGo\nNK7Wd24zUihuo034hcxjp29UJ1SCzra0zozNRP1ptQZ9Yxh5Iv0I9IfTe9miYl2A\n7SwQXvzM6d/AqT5CXRroJmrV5LbbRD5YBQLr1gjN86E6+XiqL0/+W6m5cHjF4qZa\ncSLaX1H2QZtjK1GMoWsgwrLoCiUP0vVCtBo315FduLMDdO4JCAYm6FejjqHd8pVD\noG6u+cepM66z/PvvCQD9wNnDYXGLBUX12gl1AD+f2zmafdYestkxlq2JtDUeh6ft\ngaiXcq9sbWN+EHZmaS6zvRnT9GG+8A1bEwKZ5QyTy5bDuZwPVhxUZttg+/zm9uJK\nYpANkA+UNm/cYQdsf4px3WoX2aDTYjqXc1Ixp5pOOx6zoyplqFfqnF/LOOCFBDRd\n8y2v7zrkmJOjpDQ7mrpYr0eYY1enl7/2pvW6jtWFdvlVjwJhO7YZi1zp6nv+Quy5\nrVTIxxWo4b8rjCwcmhVkocTtiBmf7tQYDNdT0bH6NJBNyIr/cy8Z26v3Tl8FtWf+\nvItx+4H8Bz6l4Nf6Ky+LJrReig3MPCpFtbT7AuZwYQR5rZJBHFPSslVlF8Z4O8Qx\nkUXGefqsg5EfXKhvvDut1iBBowkS7tsKrSsrYwEy4sHd+rvEwDJRfq+koKAEZ2YV\nzwusoXm18yQ9TX8bKFfn91U07C5umCQI8stoBTKr8u93sZu/E/kYYBWf5Nt03x6u\nNkBRby3FqgeNWsVnonMgz54iNM+zmIuCaMBI1S3WEMi3w/XuIiCfE01mJmFwaJGW\nlgb/84fiaSGCz49U68+kQqCdo8FkqyVp3gv/k7ILnsyTUMRFy195fVDjOFHZB1tO\nvkFCn9/U/2g1cmzMVRT6EhvBm70vrDSpedLhBGdnqDTKTGQkgq+GZVX7MPdlP+ie\nUh4R6oPzaF9LcbQ4pwGhPsqLZH7F3rFFdf4T7DC8/NszvgXealM3NL1F2AUDVrPi\nb4nSKZZTNDQv5HlhXr0wWhYGMCUvZZlkYHtYbi5w5qMUMJsao+V1wcwF4cUXRVeP\nFrOf0WCD3+FW8WW47GhR2ILNTSYa/SzFL460M3skrAR6FBdtTMcnmhWsTXpyeH4V\n5WaaH1m3TVwUN0wUXV+VHWL7NgIkzoLVEvCtIMnVkjmPuFxLLl9yH0jVBE3uJkqe\n/3JWBqaRyH3NPmKs2VZiVv4d6J4jrJh8ZSP1TFCUICJVDx9bUa8HvvJaUyxDi4Vd\n4wIDAQAB\n-----END PUBLIC KEY-----\n');
/*!40000 ALTER TABLE `peers` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2018-06-16  2:07:12
