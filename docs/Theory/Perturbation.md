# Perturbation Theory

The idea of perturbation is to start with the orbit of a reference point, the so called *reference orbit*, and to express the orbits of all other points in form of deviations to it. More precisely, let 
\begin{align*}
(x_n) = x_0, x_1, x_2, \ldots \tag{1}
\end{align*}
be the orbit of the reference point $x \in \mathbb{C}$ and $y$ be a point in the neighborhood of $x$. Then we can calculate the orbit of $y$ in the following form:
\begin{align*}
(y_n) = x_0 + \Delta_0, x_1 + \Delta_1, x_2 + \Delta_2, \ldots
\end{align*}
The sequence 
\begin{align*}
(\Delta_n) = \Delta_0, \Delta_1, \Delta_2, \ldots
\end{align*}
is called the *delta orbit* of $y$. Once the reference orbit $(1)$ has been calculated, it is sufficient to know the delta orbit $(\Delta_n)$ to fully reconstruct $(y_n)$.</p>

Delta orbits can be computed with a simple iteration formula that can be derived directly from the iteration formula of the Mandelbrot set:
\begin{align*}
\Delta_0 &= y_0 - x_0 \nonumber \\
\Delta_{n+1} &= y_{n+1} - x_{n+1} \nonumber \\
&= (y_n^2 + y_0) - (x_n^2 + x_0) \nonumber \\
&= (y_n^2 - x_n^2) + (y_0 - x_0)  \nonumber \\
&= ((x_n + \Delta_n)^2 - x_n^2) + \Delta_0 \nonumber \\
&= 2 x_n \Delta_n + \Delta_n^2 + \Delta_0 \tag{2}
\end{align*}
Equation $(2)$ shows that the deltas can be calculated with a sum whose summands are all very small. In fact, the numbers are so small that it is usually sufficient to represent them with low-precision data types. This outlines the core of the algorithm: Instead of slowly calculating all points in high precision, an expensive calculation of this kind is carried out for a single reference point, only. All other orbits are derived from their delta orbits which are calculated in low precision.

Unfortunately, the outlined approach does not always succeed. A general problem concerns the choice of the reference point $x$. If the orbit of $x$ escapes, i.e., if $x$ does not belong to the Mandelbrot set itself, equation $(2)$ can only be applied to the elements of the non-escaping initial segment of $(x_n)$. As a result, $x$ cannot serve as the reference for any other point $y$ that either belongs to the Mandelbrot set itself, or diverges later than $x$. Even worse, delta calculation can fail  with non-volatile reference points, too. In some cases, rounding errors lead to visible pixel errors in the generated image. Such pixels are referred to as *glitches*. If a pixel is a glitch, there is an increased probability that this is also true for its neighbors. As a result, affected image often contain areas that look extremely noisy or areas that have no detail at all, thus looking like ill-placed color blobs.

Fortunately, both problems can be solved by recomputing the affected pixels with a different reference point. Modern Mandelbrot generators thus calculate images in multiple rounds. In each round a new reference point is selected and applied to all pixels for which it turns out to be appropriate. If a glitch a detected, the pixel is set aside and revisited in the next round. This procedure is repeated until the proportion of glitch pixels drops below a predefined acceptance level.

Besides choosing a suitable reference point, we face a challenging problem. How can we decide for a point whether the currently selected reference point results in a glitch? An answer to this question is provided by the *Pauldelbrot criterion*, which was formulated in a [forum post](http://www.fractalforums.com/announcements-and-news/pertubation-theory-glitches-improvement/msg73027/#msg73027) in 2014. The criterion is based on the hypothesis that pixel errors only occur if the magnitude of $x_n + \Delta_n$ is way smaller than the magnitude of $x_n$. Symbolically, this is is denoted by:
\begin{align*}
| x_n + \Delta_n | << | x_n | \nonumber
\end{align*}
To apply the criterion in practice, a constant $\tau$ is chosen and a test added to the delta orbit calculation, which checks the following condition after each iteration:
\begin{align*}
| x_n + \Delta_n | < \tau \cdot | x_n |
\end{align*}
Once the condition hits, calculation is aborted. The affected point is put aside and revisited in the next round. Although the criterion is based on a hypothesis that is still lacking a rigid proof (as far as I know), it has turned out to work extremely well in practice. As of today, the Pauldelbrot criterion is the preferred method for glitch detection in modern Mandelbrot generators.
