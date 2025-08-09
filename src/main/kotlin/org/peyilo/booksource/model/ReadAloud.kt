package org.peyilo.booksource.model

import android.content.Context
import android.content.Intent
import android.os.Bundle
import org.peyilo.booksource.constant.AppLog
import org.peyilo.booksource.constant.EventBus
import org.peyilo.booksource.constant.IntentAction
import org.peyilo.booksource.data.appDb
import org.peyilo.booksource.data.entities.HttpTTS
import org.peyilo.booksource.help.config.AppConfig
import org.peyilo.booksource.service.BaseReadAloudService
import org.peyilo.booksource.service.HttpReadAloudService
import org.peyilo.booksource.service.TTSReadAloudService
import org.peyilo.booksource.utils.LogUtils
import org.peyilo.booksource.utils.StringUtils
import org.peyilo.booksource.utils.postEvent
import org.peyilo.booksource.utils.startForegroundServiceCompat
import org.peyilo.booksource.utils.toastOnUi
import splitties.init.appCtx

object ReadAloud {
    private var aloudClass: Class<*> = getReadAloudClass()
    val ttsEngine get() = ReadBook.book?.getTtsEngine() ?: AppConfig.ttsEngine
    var httpTTS: HttpTTS? = null

    private fun getReadAloudClass(): Class<*> {
        val ttsEngine = ttsEngine
        if (ttsEngine.isNullOrBlank()) {
            return TTSReadAloudService::class.java
        }
        if (StringUtils.isNumeric(ttsEngine)) {
            httpTTS = appDb.httpTTSDao.get(ttsEngine.toLong())
            if (httpTTS != null) {
                return HttpReadAloudService::class.java
            }
        }
        return TTSReadAloudService::class.java
    }

    fun upReadAloudClass() {
        stop(appCtx)
        aloudClass = getReadAloudClass()
    }

    fun play(
        context: Context,
        play: Boolean = true,
        pageIndex: Int = ReadBook.durPageIndex,
        startPos: Int = 0
    ) {
        val intent = Intent(context, aloudClass)
        intent.action = IntentAction.play
        intent.putExtra("play", play)
        intent.putExtra("pageIndex", pageIndex)
        intent.putExtra("startPos", startPos)
        LogUtils.d("ReadAloud", intent.toString())
        try {
            context.startForegroundServiceCompat(intent)
        } catch (e: Exception) {
            val msg = "启动朗读服务出错\n${e.localizedMessage}"
            AppLog.put(msg, e)
            context.toastOnUi(msg)
        }
    }

    fun playByEventBus(
        play: Boolean = true,
        pageIndex: Int = ReadBook.durPageIndex,
        startPos: Int = 0
    ) {
        val bundle = Bundle().apply {
            putBoolean("play", play)
            putInt("pageIndex", pageIndex)
            putInt("startPos", startPos)
        }
        postEvent(EventBus.READ_ALOUD_PLAY, bundle)
    }

    fun pause(context: Context) {
        if (BaseReadAloudService.isRun) {
            val intent = Intent(context, aloudClass)
            intent.action = IntentAction.pause
            context.startForegroundServiceCompat(intent)
        }
    }

    fun resume(context: Context) {
        if (BaseReadAloudService.isRun) {
            val intent = Intent(context, aloudClass)
            intent.action = IntentAction.resume
            context.startForegroundServiceCompat(intent)
        }
    }

    fun stop(context: Context) {
        if (BaseReadAloudService.isRun) {
            val intent = Intent(context, aloudClass)
            intent.action = IntentAction.stop
            context.startForegroundServiceCompat(intent)
        }
    }

    fun prevParagraph(context: Context) {
        if (BaseReadAloudService.isRun) {
            val intent = Intent(context, aloudClass)
            intent.action = IntentAction.prevParagraph
            context.startForegroundServiceCompat(intent)
        }
    }

    fun nextParagraph(context: Context) {
        if (BaseReadAloudService.isRun) {
            val intent = Intent(context, aloudClass)
            intent.action = IntentAction.nextParagraph
            context.startForegroundServiceCompat(intent)
        }
    }

    fun upTtsSpeechRate(context: Context) {
        if (BaseReadAloudService.isRun) {
            val intent = Intent(context, aloudClass)
            intent.action = IntentAction.upTtsSpeechRate
            context.startForegroundServiceCompat(intent)
        }
    }

    fun setTimer(context: Context, minute: Int) {
        if (BaseReadAloudService.isRun) {
            val intent = Intent(context, aloudClass)
            intent.action = IntentAction.setTimer
            intent.putExtra("minute", minute)
            context.startForegroundServiceCompat(intent)
        }
    }

}