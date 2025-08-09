package org.peyilo.booksource.model

import android.content.Context
import org.peyilo.booksource.constant.IntentAction
import org.peyilo.booksource.service.DownloadService
import org.peyilo.booksource.utils.startService

object Download {


    fun start(context: Context, url: String, fileName: String) {
        context.startService<DownloadService> {
            action = IntentAction.start
            putExtra("url", url)
            putExtra("fileName", fileName)
        }
    }

}