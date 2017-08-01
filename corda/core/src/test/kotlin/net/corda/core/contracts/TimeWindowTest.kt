package net.corda.core.contracts

import net.corda.core.utilities.millis
import net.corda.core.utilities.minutes
import org.assertj.core.api.Assertions.assertThat
import org.junit.Test
import java.time.Instant
import java.time.LocalDate
import java.time.ZoneOffset.UTC

class TimeWindowTest {
    private val now = Instant.now()

    @Test
    fun fromOnly() {
        val timeWindow = TimeWindow.fromOnly(now)
        assertThat(timeWindow.fromTime).isEqualTo(now)
        assertThat(timeWindow.untilTime).isNull()
        assertThat(timeWindow.midpoint).isNull()
        assertThat(timeWindow.contains(now - 1.millis)).isFalse()
        assertThat(timeWindow.contains(now)).isTrue()
        assertThat(timeWindow.contains(now + 1.millis)).isTrue()
    }

    @Test
    fun untilOnly() {
        val timeWindow = TimeWindow.untilOnly(now)
        assertThat(timeWindow.fromTime).isNull()
        assertThat(timeWindow.untilTime).isEqualTo(now)
        assertThat(timeWindow.midpoint).isNull()
        assertThat(timeWindow.contains(now - 1.millis)).isTrue()
        assertThat(timeWindow.contains(now)).isFalse()
        assertThat(timeWindow.contains(now + 1.millis)).isFalse()
    }

    @Test
    fun between() {
        val today = LocalDate.now()
        val fromTime = today.atTime(12, 0).toInstant(UTC)
        val untilTime = today.atTime(12, 30).toInstant(UTC)
        val timeWindow = TimeWindow.between(fromTime, untilTime)
        assertThat(timeWindow.fromTime).isEqualTo(fromTime)
        assertThat(timeWindow.untilTime).isEqualTo(untilTime)
        assertThat(timeWindow.midpoint).isEqualTo(today.atTime(12, 15).toInstant(UTC))
        assertThat(timeWindow.contains(fromTime - 1.millis)).isFalse()
        assertThat(timeWindow.contains(fromTime)).isTrue()
        assertThat(timeWindow.contains(fromTime + 1.millis)).isTrue()
        assertThat(timeWindow.contains(untilTime)).isFalse()
        assertThat(timeWindow.contains(untilTime + 1.millis)).isFalse()
    }

    @Test
    fun fromStartAndDuration() {
        val timeWindow = TimeWindow.fromStartAndDuration(now, 10.minutes)
        assertThat(timeWindow.fromTime).isEqualTo(now)
        assertThat(timeWindow.untilTime).isEqualTo(now + 10.minutes)
        assertThat(timeWindow.midpoint).isEqualTo(now + 5.minutes)
    }

    @Test
    fun withTolerance() {
        val timeWindow = TimeWindow.withTolerance(now, 10.minutes)
        assertThat(timeWindow.fromTime).isEqualTo(now - 10.minutes)
        assertThat(timeWindow.untilTime).isEqualTo(now + 10.minutes)
        assertThat(timeWindow.midpoint).isEqualTo(now)
    }
}
