package org.peyilo.booksource.utils

import android.view.MenuItem
import android.widget.ImageButton
import androidx.annotation.DrawableRes
import org.peyilo.booksource.R

fun MenuItem.setIconCompat(@DrawableRes iconRes: Int) {
    setIcon(iconRes)
    actionView?.findViewById<ImageButton>(R.id.item)?.setImageDrawable(icon)
}
