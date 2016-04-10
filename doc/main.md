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

# UART commands

## Sound

## IP Status
`ipinfo`
:   Read and display all memory mapped registers.

## IP Control
`setobjid`
:   Controls which object the IP will output information about (location, area).

## Frame buffer
`df1`
:   df1

`df2`
:   df2

`df0`
:   df0

`vf1`
:   vf1

`vf2`
:   vf2

`vf0`
:   vf0

## SD card

`lemon`
:   lemon

`heman`
:   heman

## IP modes

These commands set the control signal to a mux at the IP output, to show each
stage of image processing.

`pass`
:   Passthrough; show what the camera sees without any modifications.

`gray`
:   Grayscale.

`sobel`
:   Sobel edge detection.

`thresh`
:   Sobel passed through a threshold to get a binarized image.

`label`
:   Connected components label.

`colour`
:   Coloured connected components label.

`laser`
:   Laser detection; whatever objects the IP thinks is the laser.

`flood1`
:   First flooding stage.

`flood2`
:   Second flooding stage.

## Thresholds

`redthresh`
:   redthresh

`setsobelthresh`
:   setsobelthresh

`setflood1thresh`
:   setflood1thresh

`setflood2thresh`
:   setflood2thresh

`setminsize`
:   setminsize


## TODO uncategorized
`test_args`
:   test_args

`ipouttoggle`
:   ipouttoggle

`dummytarget`
:   dummytarget

`laseron`
:   laseron

`laseroff`
:   laseroff

`test`
:   test

`record`
:   record

`play`
:   play

`kill`
:   kill

`exit`
:   exit

`dump`
:   dump

`lowlevel`
:   lowlevel

`highlevel`
:   highlevel

`lasertest`
:   lasertest

`motortest`
:   motortest

`stop`
:   stop

`load_sounds`
:   load_sounds

`load_images`
:   load_images

`still_alive`
:   still_alive

`gun`
:   gun

`portal_gun`
:   portal_gun

`target`
:   target

`manual`
:   manual

`auto`
:   auto

`passthrough`
:   passthrough

`runip`
:   runip

`videoinfo`
:   videoinfo

