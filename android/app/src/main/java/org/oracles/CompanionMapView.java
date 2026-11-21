package org.oracles;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.RectF;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.widget.ImageView;

/** Fits the whole map by default. Tap a room to magnify it, then drag to look around. */
final class CompanionMapView extends ImageView {
    private final GestureDetector gestures;
    private final RectF artwork = new RectF();
    private int area = -1;
    private boolean zoomed;
    private float offsetX, offsetY, tapX, tapY;

    CompanionMapView(Context context) {
        super(context);
        setScaleType(ScaleType.FIT_CENTER);
        setClickable(true);
        gestures = new GestureDetector(context, new GestureDetector.SimpleOnGestureListener() {
            @Override public boolean onDown(MotionEvent event) { return true; }
            @Override public boolean onSingleTapUp(MotionEvent event) {
                tapX = event.getX();
                tapY = event.getY();
                return performClick();
            }
            @Override public boolean onScroll(MotionEvent first, MotionEvent last, float dx, float dy) {
                if (zoomed) move(offsetX - dx, offsetY - dy);
                return true;
            }
        });
    }

    void setArea(int next) {
        if (area != next) resetViewport();
        area = next;
    }

    void resetViewport() {
        zoomed = false;
        offsetX = offsetY = 0;
        invalidate();
    }

    private void move(float x, float y) {
        if (getDrawable() == null) return;
        artwork.set(getDrawable().getBounds());
        getImageMatrix().mapRect(artwork);
        offsetX = fitAxis(x, getWidth(), artwork.left * 2, artwork.right * 2);
        offsetY = fitAxis(y, getHeight(), artwork.top * 2, artwork.bottom * 2);
        invalidate();
    }

    private static float fitAxis(float offset, int viewport, float start, float end) {
        if (end - start <= viewport) return (viewport - start - end) / 2;
        return Math.max(viewport - end, Math.min(-start, offset));
    }

    @Override public boolean performClick() {
        super.performClick();
        if (getDrawable() == null) return false;
        if (zoomed) resetViewport();
        else {
            zoomed = true;
            move(getWidth() / 2f - 2 * tapX, getHeight() / 2f - 2 * tapY);
        }
        return true;
    }

    @Override public boolean onTouchEvent(MotionEvent event) {
        return gestures.onTouchEvent(event);
    }

    @Override protected void onDraw(Canvas canvas) {
        int state = canvas.save();
        if (zoomed) {
            canvas.translate(offsetX, offsetY);
            canvas.scale(2, 2);
        }
        super.onDraw(canvas);
        canvas.restoreToCount(state);
    }
}
