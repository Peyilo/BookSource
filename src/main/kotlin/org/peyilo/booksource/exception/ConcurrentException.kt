@file:Suppress("unused")

package org.peyilo.booksource.exception

/**
 * 并发限制
 */
class ConcurrentException(msg: String, val waitTime: Int) : NoStackTraceException(msg)