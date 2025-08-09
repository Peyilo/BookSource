package org.peyilo.booksource.help.storage

import cn.hutool.crypto.symmetric.AES
import org.peyilo.booksource.help.config.LocalConfig
import org.peyilo.booksource.utils.MD5Utils

class BackupAES : AES(
    MD5Utils.md5Encode(LocalConfig.password ?: "").encodeToByteArray(0, 16)
)