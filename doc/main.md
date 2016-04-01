---
title: 'TurretMaster5000 Docs'
header-includes:
    - \usepackage{fullpage}
    - \usepackage{bytefield}
    - \usepackage{graphicx}
    - \usepackage{caption}
    - \usepackage{subcaption}
    - \usepackage[section]{placeins}
---

# Register list

| address | r/w | description      |
|---------|-----|------------------|
| 5'h00   | r   | obj_m11          |
| 5'h01   | r   | obj_m12          |
| 5'h02   | r   | obj_m21          |
| 5'h03   | r   | rx_read_pointer  |
| 5'h04   | r   | tx_write_pointer |
| 5'h05   | r   | tx_read_pointer  |
| 5'h06   | r   | rx_fifo_track    |
| 5'h07   | r   | tx_fifo_track    |
| 5'h08   | r   | mm2s_tready      |
| 5'h09   | r   | mm2s_tvalid      |
| 5'h0A   | r   | s2mm_tvalid      |
| 5'h0B   | r   | s2mm_tready      |
| 5'h0C   | w   | ctrl_reg1        |
| 5'h0D   | w   | frame_resetn     |
| 5'h0E   | r   | laser_xy         |
| 5'h0F   | w   | ctrl_reg2        |
| 5'h10   | r   | num_labels       |
| 5'h11   | r   | obj_area         |
| 5'h12   | r   | obj_x            |
| 5'h13   | r   | obj_y            |
| 5'h14   | r   | obj_m20          |
| 5'h15   | r   | obj_m02          |
| 5'h16   | r   | obj_m30          |
| 5'h17   | r   | obj_m03          |

: Table of AXI registers.  'w' indicates read and write register, 'r'
indicates read only register.

\begin{figure}
    \centering

    \begin{subfigure}{\textwidth}
        \centering
        \begin{bytefield}[endianness=big]{32}
            \bitheader{31, 23, 15, 7, 0} \\
            \bitbox{8}{flood threshold} 
            & \bitbox{8}{red threshold}
            & \bitbox{8}{sobel threshold}
            & \bitbox{8}{output mode}
            \\
        \end{bytefield}
        \caption{Control register 1.}
    \end{subfigure}

    \par\bigskip
    
    \begin{subfigure}{\textwidth}
        \centering
        \begin{bytefield}[endianness=big]{32}
            \bitheader{31, 23, 15, 11, 0} \\
            \bitbox{8}{flood 2 threshold} 
            & \bitbox{8}{input only}
            & \bitbox{4}{unused}
            & \bitbox{12}{object id}
            \\
        \end{bytefield}
        \caption{Control register 2.}
    \end{subfigure}

    \caption{Control register layout.}
\end{figure}


# Mode enums

| i | mode    |
|---|---------|
| 0 | pass    |
| 1 | gray    |
| 2 | sobel   |
| 3 | thresh  |
| 7 | flood1  |
| 8 | flood2  |
| 4 | cc      |
| 5 | color   |
| 6 | laser   |

# Moment calculations

## Translational invariants
$$
u_{ij} = \sum_x \sum_y (x - \bar{x}) ^ i (y - \bar{y}) ^ j p_{ij}
$$

## Scale invariants
$$
n_{ij} = \frac{u_{ij}}{u_{00} ^ {1 + \frac{i + j}{2}}}
$$

## Rotational invariants

$$
\begin{aligned}
I_0 &= n_{20} + n_{02} \\
I_1 &= (n_{20} - n_{02}) ^ 2 + 4 n_{11}^2 \\
I_2 &= (n_{30} - 3 n_{12}) ^ 2 + (3 n_{21} - n_{03}) ^ 2 \\
I_3 &= (n_{30} + n_{12}) ^ 2 + (n_{32} + n_{03}) ^ 2 \\
I_4 &= (n_{30} - 3 n_{12}) (n_{30} + n_{12})
        \left((n_{30} + n_{12}) ^ 2 - 3 (n_{21} + n_{03}) ^ 2\right) \\
        &+ (3 n_{21} - n_{03}) (n_{21} + n_{03})
        \left(3 (n_{30} + n_{12}) ^ 2 - (n_{21} + n_{03}) ^ 2\right) \\
I_5 &= (n_{20} - n_{02})
        \left((n_{30} + n_{12}) ^ 2 - (n_{21} + n_{03}) ^ 2\right) \\
        &+ 4 n_{11} (n_{30} + n_{12}) (n_{21} + n_{03}) \\
I_6 &= (3 n_{21} - n_{03}) (n_{30} + n_{12})
        \left((n_{30} + n_{12}) ^ 2 - 3 (n_{21} + n_{03}) ^ 2 \right) \\
        &- (n_{30} - 3 n_{12}) (n_{21} + n_{03})
        \left(3 (n_{30} + n_{12}) ^ 2 - (n_{21} + n_{03}) ^ 2 \right)
\end{aligned}
$$

\newpage

## Comparison

$$
x = \sum_{i=0}^5 \frac{(I_i - I^\prime_i) ^ 2}{I_i I^\prime_i}
$$
