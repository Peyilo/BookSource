package android.text

object TextUtils {

    @JvmStatic
    fun isEmpty(str: CharSequence?): Boolean {
        return str == null || str.isEmpty()
    }

    @JvmStatic
    fun join(delimiter: CharSequence, tokens: Iterable<*>): String {
        return tokens.joinToString(delimiter)
    }

    @JvmStatic
    fun join(delimiter: CharSequence, tokens: Array<*>): String {
        return tokens.joinToString(delimiter)
    }

}