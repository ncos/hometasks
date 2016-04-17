#!/bin/bash
scp -P 60001 -r ./ s21607@remote.vdi.mipt.ru:~
ssh -p 60001 s21607@remote.vdi.mipt.ru
