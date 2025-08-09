package org.peyilo.booksource.help.glide.progress

typealias OnProgressListener = ((isComplete: Boolean, percentage: Int, bytesRead: Long, totalBytes: Long) -> Unit)?